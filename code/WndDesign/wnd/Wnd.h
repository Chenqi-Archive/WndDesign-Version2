#pragma once

#include "WndObject.h"
#include "../geometry/margin.h"
#include "../common/list_iterator.h"
#include "../style/wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


class Wnd : public WndObject {
public:
	using Style = WndStyle;

public:
	Wnd(unique_ptr<Style> style);
	~Wnd();


	//// window style ////
private:
	unique_ptr<Style> _style;
protected:
	Style& GetStyle() { return *_style; }
protected:
#error is it needed? may be useful when areo snap desktop directly set region.
	virtual void OnSizeChange(Rect accessible_region) override;

	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) override;
	virtual const wstring& GetTitle() const override;


	//// client region ////
private:
	Margin _non_client_margin_without_padding;  // used for client hit-test and scrollbar drawing
	Margin _non_client_margin;
	Rect _client_region;
public:
#error the coordinate of accessible region may be the same as client region
	const Vector GetClientOffset() const {
		return Vector(_non_client_margin.left, _non_client_margin.top);
	}
	const Rect GetClientRegion() const { 
		return _client_region; 
	}


	//// invalid layout ////
private:
	struct InvalidLayout {
		bool region_on_parent;
		bool non_client_margin;
		bool client_region;
		bool content_layout;
	};
	InvalidLayout _invalid_layout = { true, true, true, true };
	virtual bool HasInvalidLayout() const override {
		return _invalid_layout.region_on_parent || _invalid_layout.non_client_margin || 
			_invalid_layout.client_region || _invalid_layout.content_layout;
	}
protected:
	enum class LayoutRegion {
		RegionOnParent,
		NonClientMargin,
		ClientRegion,
		ContentLayout,
	};
	constexpr void LayoutChanged(LayoutRegion layout_region) {
		switch (layout_region) {
		case LayoutRegion::RegionOnParent: _invalid_layout.region_on_parent = true;	break;
		case LayoutRegion::NonClientMargin: _invalid_layout.non_client_margin = true; break;
		case LayoutRegion::ClientRegion: _invalid_layout.client_region = true; break;
		case LayoutRegion::ContentLayout: _invalid_layout.content_layout = true; break;
		default: throw std::invalid_argument("invalid layout type");
		}
		JoinReflowQueue();
	}
	void RegionOnParentChanged() { LayoutChanged(LayoutRegion::RegionOnParent); }
	void NonClientMarginChanged() { LayoutChanged(LayoutRegion::NonClientMargin); }
	void ClientRegionChanged() { LayoutChanged(LayoutRegion::ClientRegion); }
	void ContentLayoutChanged() { LayoutChanged(LayoutRegion::ContentLayout); }
public:
	/* called by child window whose layout has changed and may influence my layout */
	virtual void ChildLayoutChanged(WndObject& child) override { /*ContentLayoutChanged();*/ }

private:
	/* called by reflow queue for the first time to check if parent's layout may be influenced */
	// returns true if region_on_parent may change, and reflow queue will add parent window in.
	virtual bool MayRegionOnParentChange() override {

	}

	/* called by reflow queue when finally updating */
	virtual void UpdateInvalidLayout() override {

	}

	/* called by child window whose region is about to change */
	virtual void CalculateChildRegion(WndObject& child) override {

	}

	/* called by parent window when parent is updating */
	virtual const Rect UpdateRegionOnParent(Size parent_size) override {
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


	//// message handling ////
private:
	virtual void NonClientHitTest(Point point);


};


END_NAMESPACE(WndDesign)