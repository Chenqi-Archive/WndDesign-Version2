#pragma once

#include "WndObject.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;


class OverlapLayout : public WndObject {
private:
	struct ChildWndContainer {
		WndObject& wnd;
		Rect region; 
		list<ChildWndContainer>::iterator list_index;
	};
	// frontmost(begin) ---> endmost(end)
	list<ChildWndContainer> _child_wnds;  // or use spatial index instead.

private:
	const Size GetClientSize() {

	}

public:
	void AddChild(WndObjectBase& child) {    // z-index
		RegisterChild(child);
		Rect child_region = child.CalculateRegion(GetSizeAsParent());
		SetChildRegion(child, child_region);
		ChildWndContainer& child_container = _child_wnds.emplace_front(ChildWndContainer{ child, child_region });
		child_container.list_index = _child_wnds.begin();
		SetChildData<ChildWndContainer*>(child, &child_container);
		Invalidate(child_region);
	}
	virtual void OnChildDetach(WndObjectBase& child) override {
		ChildWndContainer& child_container = *GetChildData<ChildWndContainer*>(child);
		Rect child_region = child_container.region;
		_child_wnds.erase(child_container.list_index);
		Invalidate(child_region);
	}

private:
	virtual void OnSizeChange(Rect accessible_region) override {


		Invalidate(accessible_region)
	}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) override {}
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) override {}

private:
	virtual void OnChildRegionChange(WndObjectBase& child) override {
		child.CalculateRegion(GetSizeAsParent())
	}
	virtual const Rect CalculateRegionOnParent(Size parent_size) { return region_empty; }
	virtual const Rect CalculateAccessibleRegion(Size parent_size, Size display_region_size) { return Rect(point_zero, display_region_size); }

private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}

private:
	virtual bool Handler(Msg msg, Para para) override { return true; }
	virtual const WndObjectBase& HitTestChild(Point point) const override { return *this; }
	virtual bool HitTest(Point point) const override { return true; }
};


END_NAMESPACE(WndDesign)