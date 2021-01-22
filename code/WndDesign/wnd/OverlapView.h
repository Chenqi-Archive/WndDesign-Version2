#pragma once

#include "WndObject.h"


BEGIN_NAMESPACE(WndDesign)


class OverlapView : public WndObject {
private:
	struct ChildWndContainer {
		WndObject& wnd;
		Rect region;
	};
	list<ChildWndContainer> _children;  // or other spatial indexes.

public:
	void AddChild(WndObject& child) {    // z-index
		RegisterChild(child);
		Rect child_region = child.CalculateRegion(GetSizeAsParent());
		SetChildRegion(child, child_region);
		_children.push_back({ child, child_region });
		Invalidate(child_region);
		sizeof(list<ChildWndContainer>::iterator);
	}
	void RemoveChild(WndObject& child) {
		Rect child_region = child.GetRegionOnParnet();
		UnregisterChild(child);
		Invalidate(child_region);
	}

private:
	virtual void OnSizeChange(Rect accessible_region) override {


		Invalidate(accessible_region)
	}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) override {}
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) override {}

private:
	virtual const Rect OnChildRegionChange(WndObject& child, Rect child_region) { return child_region; }
	virtual const Rect CalculateRegionOnParent(Size parent_size) { return region_empty; }
	virtual const Rect CalculateAccessibleRegion(Size parent_size, Size display_region_size) { return Rect(point_zero, display_region_size); }

private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}

private:
	virtual bool Handler(Msg msg, Para para) { return true; }
	virtual const WndObject& HitTestChild(Point point) const { return *this; }
	virtual bool HitTest(Point point) const { return true; }
};


END_NAMESPACE(WndDesign)