#pragma once

#include "wnd_base.h"  // use interface instead.
#include "../common/uncopyable.h"
#include "../geometry/geometry.h"
#include "../message/msg_base.h"
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class WndBase;


class WndObject : public Uncopyable {
private:
	unique_ptr<WndBase> wnd;

protected:
	WndObject() : wnd(std::make_unique<WndBase>(*this)) {}
	~WndObject() {}


	//// wrapper functions of WndBase interface ////
public:
	bool HasParent() const { return wnd->HasParent(); }
	WndObject& GetParent() const { return wnd->GetParent(); }
	const Rect GetAccessibleRegion() const { return wnd->GetAccessibleRegion(); }
	const Rect GetDisplayRegion() const { return wnd->GetDisplayRegion(); }
	void SetBackground(const Background& background) { wnd->SetBackground(background); }
	const Vector SetDisplayOffset(Point display_offset) { return wnd->SetDisplayOffset(display_offset); }
	void Invalidate(Rect region) { wnd->Invalidate(region); }
	void AddChild(WndObject& child) { wnd->AddChild(*child.wnd); }
	void RemoveChild(WndObject& child) { wnd->RemoveChild(*child.wnd); }
public:
	const Size GetSize() const { return GetAccessibleRegion().size; }
	const Point GetDisplayOffset() const { return GetDisplayRegion().point; }
	const Vector ScrollView(Vector vector) { return SetDisplayOffset(GetDisplayOffset() + vector); }  // returns the real vector shifted
private:
	void SetAccessibleRegion(Rect accessible_region) { wnd->SetAccessibleRegion(accessible_region); }
	void SetRegionOnParent(Rect region_on_parent) { wnd->SetRegionOnParent(region_on_parent); }
protected:
	/* called by parent window when parent window is painting */
	void Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const { wnd->Composite(figure_queue, parent_invalid_region); }


	//// callback functions called by WndBase ////
private:
	friend class WndBase;
	virtual bool Handler(Msg msg, Para para) { return true; }
	virtual void OnSizeChange(Rect accessible_region) {}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) {}  // for scrolling
	virtual void OnVisibleRegionChange(Rect accessible_region, Rect visible_region) {}  // for lazy-loading
	/* draw anything to the invalid region except for background */
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}
	virtual const Background& GetBackground() const {}


	//// region computing ////
private:
	void UpdateChildRegion(WndObject& child) {
		Rect child_region_on_parent = child.CalculateRegionOnParent(GetSize());
		child_region_on_parent = OnChildRegionChange(child, child_region_on_parent);
		child.SetRegionOnParent(child_region_on_parent);
	}
	const Rect CalculateRegionOnParent(Size parent_size) { 
		Rect display_region = CalculateDisplayRegion(parent_size);
		Rect accessible_region = CalculateAccessibleRegion(parent_size, display_region);
		SetAccessibleRegion(accessible_region);
		return display_region;
	}
public:
	void RegionUpdated() { if (HasParent()) { GetParent().UpdateChildRegion(*this); } }
private:
	virtual const Rect CalculateDisplayRegion(Size parent_size) { return region_empty; }
	virtual const Rect CalculateAccessibleRegion(Size parent_size, Rect display_region) { return Rect(point_zero, display_region.size); }
	virtual const Rect OnChildRegionChange(WndObject& child, Rect child_display_region) { return child_display_region; }
};


END_NAMESPACE(WndDesign)