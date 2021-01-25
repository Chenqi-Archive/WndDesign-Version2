#pragma once

#include "../geometry/geometry.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class WndObjectBase;
class FigureQueue;
struct Background;


struct ABSTRACT_BASE IWndBase {
	WNDDESIGNCORE_API static unique_ptr<IWndBase> Create(WndObjectBase& object);

	virtual ~IWndBase() pure {}

	//// child and parent window relation ////
	virtual ref_ptr<WndObjectBase> GetParent() const pure;
	virtual void AddChild(IWndBase& child) pure;
	virtual void RemoveChild(IWndBase& child) pure;

	//// window region ////
	virtual const Rect GetAccessibleRegion() const pure;
	virtual const Rect GetDisplayRegion() const pure;
	virtual const Rect GetRegionOnParent() const pure;
	virtual void SetAccessibleRegion(Rect accessible_region) pure;
	virtual const Vector SetDisplayOffset(Point display_offset) pure;
	virtual void SetRegionOnParent(Rect region_on_parent) pure;

	//// painting and composition ////
	virtual void SetBackground(const Background& background) pure;
	virtual void AllocateLayer() pure;
	virtual void Invalidate(Rect region) pure;
	virtual void Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const pure;

	//// message handling ////
	virtual void SetCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseCapture() pure;
	virtual void ReleaseFocus() pure;
};


END_NAMESPACE(WndDesign)