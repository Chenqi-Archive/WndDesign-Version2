#pragma once

#include "../geometry/geometry.h"
#include "../message/msg_base.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class WndObject;
class FigureQueue;
struct Background;

inline static const uint max_wnd_depth = 63;  // depth (valid) <= 63


struct ABSTRACT_BASE IWndBase {
	WNDDESIGNCORE_API static unique_ptr<IWndBase> Create(WndObject& object);

	virtual ~IWndBase() pure {}

	//// child and parent window relation ////
	virtual void AddChild(IWndBase& child) pure;
	virtual void RemoveChild(IWndBase& child) pure;

	//// window region ////
	virtual const Rect GetAccessibleRegion() const pure;
	virtual const Rect GetDisplayRegion() const pure;
	virtual const Rect GetRegionOnParent() const pure;
	virtual void SetAccessibleRegion(Rect accessible_region) pure;
	virtual const Vector SetDisplayOffset(Point display_offset) pure;
	virtual void SetRegionOnParent(Rect region_on_parent) pure;

	//// layout update ////
	virtual void InvalidateLayout() pure;

	//// painting and composition ////
	virtual void SetBackground(const Background& background) pure;
	virtual void AllocateLayer() pure;
	virtual void Invalidate(Rect region) pure;
	virtual void InvalidateChild(IWndBase& child, Rect child_invalid_region) pure;
	virtual void Composite(FigureQueue& figure_queue, Vector client_offset, Rect parent_invalid_region) const pure;

	//// message handling ////
	virtual void SetCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseCapture() pure;
	virtual void ReleaseFocus() pure;
	virtual bool SendChildMessage(IWndBase& child_wnd, Msg msg, Para para) pure;
};


END_NAMESPACE(WndDesign)