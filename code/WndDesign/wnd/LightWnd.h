#pragma once

#include "WndObject.h"


BEGIN_NAMESPACE(WndDesign)


// Light-weight window base class, which is simple and effective and should have no child window.
// LightWnd should be attached to a heavy-weight WndObject parent to work, and is fully managed by its parent.
// In fact, all kinds of FinalWnd can be implemented as LightWnd.
class LightWnd {
private:
	ref_ptr<WndObject> _parent;

private:
	Rect _region_on_parent;
protected:
	void SetRegionOnParent(Rect region_on_parent) { _region_on_parent = region_on_parent; }
public:
	const Size GetSize() const { return _region_on_parent.size; }
	const Rect GetRegionOnParent() const { return _region_on_parent; }
	// point_on_parent + offset_from_parent = point_on_myself
	const Vector OffsetFromParent() const { return point_zero - _region_on_parent.point; }
private:
	virtual void UpdateRegionOnParent(Size parent_size) {}
private:
	void RegionChanged() {}


protected:
	void Invalidate(Rect region) { _parent->Invalidate(_region_on_parent.Intersect(region - OffsetFromParent())); }
private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect invalid_region) const {}

private:
	void SetCapture() { _parent->SetCapture(); }
	void SetFocus() { _parent->SetFocus(); }
	void ReleaseCapture() { _parent->ReleaseCapture(); }
	void ReleaseFocus() { _parent->ReleaseFocus(); }
public:
	virtual bool HitTest(Point point) const { return true; }
	virtual bool Handler(Msg msg, Para para) { return true; }
};


END_NAMESPACE(WndDesign)