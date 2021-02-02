#pragma once

#include "WndObject.h"
#include "../geometry/margin.h"


BEGIN_NAMESPACE(WndDesign)

struct WndStyle;


class Wnd : public WndObject {
public:
	using Style = WndStyle;

public:
	Wnd(unique_ptr<Style> style);
	~Wnd();


	//// window style and region calculation ////
private:
	unique_ptr<Style> _style;
protected:
	Style& GetStyle() { return *_style; }
protected:
	virtual void OnSizeChange(Rect accessible_region) override;
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) override;
	virtual const wstring& GetTitle() const override;


private:
	Margin _non_client_margin_without_padding;
	Margin _non_client_margin;

	Rect _client_region;

private:
	void CalculateNonClientMargin();

public:
	const Vector GetClientOffset() const {
		return Vector(_non_client_margin.left, _non_client_margin.top);
	}

	const Rect GetClientRegion(Rect accessible_region) const;


	// reflow queue

private:
	void JoinReflowQueue() {}
	void LeaveReflowQueue() {}
protected:
	void LayoutChanged() { JoinReflowQueue(); }
	void UpdateLayout() { 

		WndObject::LayoutChanged(); 
	}
public:
	virtual const Rect UpdateLayout(Size parent_size) override { 
		// calculate and set my region

		// leave reflow queue

		// returns region on parent
	}


	// child windows
protected:
	void SetChildRegion(WndObject& child, Rect region) {
		WndObject::SetChildRegion(child, region + GetClientOffset());
	}
	const Rect GetChildRegion(WndObject& child) {
		return WndObject::GetChildRegion(child) - GetClientOffset();
	}

	//// painting and composition ////
protected:
	void CompositeChild(WndObject& child, FigureQueue& figure_queue, Rect invalid_client_region) {
		WndObject::CompositeChild(child, figure_queue, invalid_client_region + GetClientOffset());
	}

	void Invalidate(Rect invalid_client_region) {
		WndObject::Invalidate(invalid_client_region + GetClientOffset());
	}

	void NonClientInvalidate(Rect invalid_non_client_region) {
		if (HasParent()) { GetParent()->InvalidateChild(*this, invalid_non_client_region); }
	}

private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const override /*final*/;
private:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {}
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const override;
};


END_NAMESPACE(WndDesign)