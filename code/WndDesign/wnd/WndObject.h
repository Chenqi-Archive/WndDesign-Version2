#pragma once

#include "wnd_base_interface.h"
#include "../common/uncopyable.h"
#include "../message/msg_base.h"


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::pair;


class WndObject : public Uncopyable {
private:
	friend class WndBase;
	unique_ptr<IWndBase> wnd;

protected:
	WndObject() : wnd(IWndBase::Create(*this)) {}
	~WndObject() {}


	//// child and parent window relation ////
public:
	bool HasParent() const { return wnd->HasParent(); }
	WndObject& GetParent() const { return wnd->GetParent(); }
protected:
	void RegisterChild(WndObject& child) { wnd->AddChild(*child.wnd); }
	void UnregisterChild(WndObject& child) { wnd->RemoveChild(*child.wnd); }


	//// window region ////
public:
	const Rect GetAccessibleRegion() const { return wnd->GetAccessibleRegion(); }
	const Size GetSize() const { return GetAccessibleRegion().size; }
	const Rect GetDisplayRegion() const { return wnd->GetDisplayRegion(); }
	const Vector SetDisplayOffset(Point display_offset) { return wnd->SetDisplayOffset(display_offset); }
	const Point GetDisplayOffset() const { return GetDisplayRegion().point; }
	const Vector ScrollView(Vector vector) { return SetDisplayOffset(GetDisplayOffset() + vector); }  // returns the real vector shifted
private:
	void SetAccessibleRegion(Rect accessible_region) { wnd->SetAccessibleRegion(accessible_region); }
	void SetRegionOnParent(Rect region_on_parent) { wnd->SetRegionOnParent(region_on_parent); }
private:
	virtual void OnSizeChange(Rect accessible_region) {}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) {}  // for scrolling
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) {}  // for lazy-loading


	//// region computing ////
private:
	void UpdateChildRegion(WndObject& child) {
		Rect child_region = child.CalculateRegion(GetSize());
		child_region = OnChildRegionChange(child, child_region);
		child.SetRegionOnParent(child_region);
	}
	const Rect CalculateRegion(Size parent_size) {
		Rect region_on_parent = CalculateRegionOnParent(parent_size);
		Rect accessible_region = CalculateAccessibleRegion(parent_size, region_on_parent.size);
		SetAccessibleRegion(accessible_region);
		return region_on_parent;
	}
public:
	void RegionUpdated() { if (HasParent()) { GetParent().UpdateChildRegion(*this); } }
private:
	virtual const Background& GetBackground() const { return NullBackground(); }
	virtual const Rect OnChildRegionChange(WndObject& child, Rect child_region) { return child_region; }
	virtual const Rect CalculateRegionOnParent(Size parent_size) { return region_empty; }
	virtual const Rect CalculateAccessibleRegion(Size parent_size, Size display_region_size) { return Rect(point_zero, display_region_size); }


	//// other window styles ////
private:
	// used for desktop window.
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { return { size_min, size_max }; }
	virtual const wstring GetTitle() const { return wstring(); }


	//// painting and composition ////
public:
	void SetBackground(const Background& background) { wnd->SetBackground(background); }
	void AllocateLayer() { wnd->AllocateLayer(); }
	void Invalidate(Rect region) { wnd->Invalidate(region); }
protected:
	/* called by parent window when parent window is painting */
	void Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const { wnd->Composite(figure_queue, parent_invalid_region); }
private:
	/* draw anything to the invalid region except for background */
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}


	//// message handling ////
public:
	void SetCapture() { wnd->SetCapture(); }
	void SetFocus() { wnd->SetFocus(); }
	void ReleaseCapture() { wnd->ReleaseCapture(); }
	void ReleaseFocus() { wnd->ReleaseFocus(); }
private:
	virtual bool Handler(Msg msg, Para para) { return true; }
	virtual const WndObject& HitTestChild(Point point) const { return *this; }
	virtual bool HitTest(Point point) const { return true; }
};


END_NAMESPACE(WndDesign)