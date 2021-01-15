#include "wnd_base.h"
#include "redraw_queue.h"


BEGIN_NAMESPACE(WndDesign)


WndBase::WndBase(WndObject& object) :
	_object(object),
	_parent(nullptr), _index_on_parent(),
	_child_wnds(),

	_accessible_region(region_empty), 
	_display_offset(point_zero),
	_display_region(region_empty),
	_visible_region(region_empty),

	_layer(),

	_depth(0),
	_redraw_queue_index(),
	_invalid_region() 
#pragma error /* capture */
{
}

WndBase::~WndBase() {
	LeaveRedrawQueue();
	DetachFromParent();
}

void WndBase::SetParent(ref_ptr<WndBase> parent, list<WndBase&>::iterator index_on_parent) {
	DetachFromParent();
	_parent = parent; _index_on_parent = index_on_parent;

	// Set depth.

	// If has invalid region, join figure queue.

	// Set visible region.
}

void WndBase::ClearParent() {
	_parent = nullptr;
	_index_on_parent = {};
}

void WndBase::DetachFromParent() {
	if (HasParent()) { _parent->RemoveChild(*this); }

	// reset depth

}

void WndBase::AddChild(WndBase& child_wnd) {

	// set child depth, display region and visible region.

}

void WndBase::RemoveChild(WndBase& child_wnd) {
	if (child_wnd._parent != this) { return; }
	_child_wnds.erase(child_wnd._index_on_parent);
	child_wnd.ClearParent();

	// clear child depth and visible region.

	// Clear message tracked windows.
}

void WndBase::SetDepth(uint depth) {
	if (_depth == depth) { return; }
	// Depth has changed, re-enter redraw queue.
	LeaveRedrawQueue();
	_depth = depth;

	// Set depth for child windows.

	// Enter redraw queue, if depth > 0 && invalid region is not empty.
	if (_depth > 0 && !_invalid_region.IsEmpty()) {

	}
}

void WndBase::SetAccessibleRegion(Rect accessible_region) {
	_accessible_region = accessible_region;

	// OnSizeChange.

}

void WndBase::SetDisplayOffset(Point display_offset) {

	// Send scroll message.

}

void WndBase::SetDisplayRegion(Rect display_region) {
	_display_region = display_region;

	// Set visible region.
	SetVisibleRegion(GetParentCachedRegion());

	// Send scroll message.
}

void WndBase::SetVisibleRegion(Rect visible_region) {
	_visible_region = visible_region;
	if (HasLayer()) {
		GetLayer().SetCachedRegion(_visible_region);

		// invalidate new cached region.
	}

	// Set visible region for child windows.

}

void WndBase::Invalidate(const Region& region) {
	_invalid_region.Union(region);
	JoinRedrawQueue();
}

void WndBase::Invalidate(Rect region) {
	region = region.Intersect(GetCachedRegion());
	if (!region.IsEmpty()) {
		_invalid_region.Union(region);
		JoinRedrawQueue();
	}
}

void WndBase::JoinRedrawQueue() {
	if (_depth > 0 && !HasRedrawQueueIndex()) {
		RedrawQueue::Get().AddWnd(*this);
	}
}

void WndBase::LeaveRedrawQueue() {
	if (HasRedrawQueueIndex()) {
		RedrawQueue::Get().RemoveWnd(*this);
	}
}

void WndBase::UpdateInvalidRegion() {
	// First check if has parent window, reset depth when detached from parent.
	// But not erase the invalid region.
	if (!HasParent()) { SetDepth(0); return; }

	// Clip invalid region inside accessible region.
	_invalid_region.Intersect(_accessible_region);
	if (_invalid_region.IsEmpty()) { return; }

	// Draw figure queue to layer.
	if (HasLayer()) {
		auto [bounding_region, regions] = _invalid_region.GetRect();
		FigureQueue figure_queue;
		figure_queue.Append(vector_zero, new BackgroundFigure(_background, bounding_region, true));
		_object.OnPaint(figure_queue, _accessible_region, bounding_region);
		for (auto& region : regions) {
			GetLayer().DrawFigureQueue(figure_queue, region);
		}
	}

	// Invalidate region for parent window.
	_invalid_region.Translate(_display_offset - _display_region.point);
	_invalid_region.Intersect(_display_region);
	_parent->Invalidate(_invalid_region);

	// Clear invalid region.
	_invalid_region.Clear();
}

void WndBase::Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const {
	// Convert parent invalid region to my invalid region.
	Rect invalid_region = parent_invalid_region.Intersect(_display_region) + (_display_offset - _display_region.point);
	if (HasLayer()) {
		figure_queue.Append(invalid_region.point - point_zero, new LayerFigure(GetLayer(), invalid_region, {}));
	} else {
		// Push region clip.

		figure_queue.Append(vector_zero, new BackgroundFigure());
		// (Call WndObject)
		// Draw background.
		// Draw child windows.

		// Pop region clip.
	}
}


END_NAMESPACE(WndDesign)