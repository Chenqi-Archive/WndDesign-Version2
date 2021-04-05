#pragma once

#include "Wnd.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;


class OverlapLayout : public Wnd {
public:
	using Style = Wnd::Style;

public:
	OverlapLayout(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~OverlapLayout() {}


	//// child windows ////
private:
	struct ChildWndContainer {
		WndObject& wnd;
		Rect region;
		CompositeEffect composite;
		list<ChildWndContainer>::iterator list_index;
	};

	// frontmost(begin) ---> endmost(end)
	list<ChildWndContainer> _child_wnds;  // or use spatial index instead

private:
	static void SetChildData(WndObject& child, ChildWndContainer& child_container) {
		WndObject::SetChildData<ChildWndContainer*>(child, &child_container);
	}
	static ChildWndContainer& GetChildData(WndObject& child) {
		return *WndObject::GetChildData<ChildWndContainer*>(child);
	}

private:
	void InsertChild(WndObject& child, Rect region);
	const Rect EraseChild(WndObject& child);

public:
	void AddChild(WndObject& child);

protected:
	virtual void OnChildDetach(WndObject& child) override;
	virtual void OnChildCompositeEffectChange(WndObject& child);


	//// layout update ////
private:
	void UpdateChildRegion(ChildWndContainer& child_container, Size client_size);
private:
	virtual void OnChildRegionUpdate(WndObject& child) override;
	virtual const Rect UpdateContentLayout(Size client_size) override;


	//// painting and composition ////
private:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const override;


	//// message handling ////
protected:
	virtual const Wnd::HitTestInfo ClientHitTest(Size client_size, Point point) const override;
};


END_NAMESPACE(WndDesign)