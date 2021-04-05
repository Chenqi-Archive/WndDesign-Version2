#include "OverlapLayout.h"
#include "../common/reversion_wrapper.h"


BEGIN_NAMESPACE(WndDesign)


void OverlapLayout::InsertChild(WndObject& child, Rect region) {
	CompositeEffect composite = child.GetCompositeEffect();
	auto next_iter_pos = std::find_if(
		_child_wnds.begin(), _child_wnds.end(),
		[=](const ChildWndContainer& child_container) { return child_container.composite._z_index <= composite._z_index; }
	);
	auto iter_pos = _child_wnds.emplace(next_iter_pos, ChildWndContainer{ child, region, composite });
	iter_pos->list_index = iter_pos;
	SetChildData(child, *iter_pos);
}

const Rect OverlapLayout::EraseChild(WndObject& child) {
	ChildWndContainer& child_container = GetChildData(child);
	Rect region = child_container.region;
	_child_wnds.erase(child_container.list_index);
	return region;
}

void OverlapLayout::AddChild(WndObject& child) {
	RegisterChild(child);
	InsertChild(child, region_empty);
}

void OverlapLayout::OnChildDetach(WndObject& child) {
	Wnd::OnChildDetach(child);
	Rect region = EraseChild(child);
	Invalidate(region);
}

void OverlapLayout::OnChildCompositeEffectChange(WndObject& child) {
	Rect region = EraseChild(child);
	InsertChild(child, region);
	Invalidate(region);
}

void OverlapLayout::UpdateChildRegion(ChildWndContainer& child_container, Size client_size) {
	Rect child_region = WndObject::UpdateChildRegion(child_container.wnd, client_size);
	if (child_container.region != child_region) {
		Invalidate(child_container.region);
		Invalidate(child_region);
		child_container.region = child_region;
		SetChildRegion(child_container.wnd, child_region);
	}
}

void OverlapLayout::OnChildRegionUpdate(WndObject& child) {
	ChildWndContainer& child_container = GetChildData(child);
	UpdateChildRegion(child_container, GetClientSize());
}

const Rect OverlapLayout::UpdateContentLayout(Size client_size) {
	if (client_size != GetClientSize()) {
		for (auto& child_container : _child_wnds) {
			UpdateChildRegion(child_container, client_size);
		}
	}
	return Rect(point_zero, client_size);
}

void OverlapLayout::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {
	for (auto& child_container : reverse(_child_wnds)) {
		Rect invalid_child_region = child_container.region.Intersect(invalid_client_region);
		if (!invalid_child_region.IsEmpty()) {
			CompositeChild(child_container.wnd, figure_queue, invalid_child_region);
		}
	}
}

const Wnd::HitTestInfo OverlapLayout::ClientHitTest(Size client_size, Point point) const { 
	for (auto& container : _child_wnds) {
		if (!container.composite._mouse_penetrate && container.region.Contains(point)){
			Point point_on_child = point - (container.region.point - point_zero);
			if (container.wnd.NonClientHitTest(container.region.size, point_on_child)) {
				return { &container.wnd, point_on_child };
			}
		}
	}
	return {}; 
}


END_NAMESPACE(WndDesign)