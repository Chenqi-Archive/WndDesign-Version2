#include "OverlapLayout.h"
#include "../common/reversion_wrapper.h"
#include "../figure/figure_queue.h"


BEGIN_NAMESPACE(WndDesign)


void OverlapLayout::AddChild(WndObject& child) {
	RegisterChild(child);
	ChildWndContainer& child_container = _child_wnds.emplace_front(ChildWndContainer{ child, region_empty });
	child_container.list_index = _child_wnds.begin();
	SetChildData(child, child_container);
}

void OverlapLayout::OnChildDetach(WndObject& child) {
	ChildWndContainer& child_container = GetChildData(child);
	Invalidate(child_container.region);
	_child_wnds.erase(child_container.list_index);
}

void OverlapLayout::OnChildRegionUpdate(WndObject& child) {
	auto& child_container = GetChildData(child);
	Rect child_region = UpdateChildRegion(child, GetClientSize());
	if (child_container.region != child_region) {
		Invalidate(child_container.region);
		Invalidate(child_region);
		child_container.region = child_region;
	}
}

const Rect OverlapLayout::UpdateContentLayout(Size client_size) {
	if (client_size == GetClientSize()) {
	} else {
		for (auto& child_container : _child_wnds) {
			Rect child_region = UpdateChildRegion(child_container.wnd, client_size);
			if (child_container.region != child_region) {
				Invalidate(child_container.region);
				Invalidate(child_region);
				child_container.region = child_region;
			}
		}
	}
	return Rect(point_zero, client_size);
}

void OverlapLayout::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {
	for (auto& child_container : reverse(_child_wnds)) {
		if (!child_container.region.Intersect(invalid_client_region).IsEmpty()) {
			CompositeChild(child_container.wnd, figure_queue, invalid_client_region);
		}
	}
}


END_NAMESPACE(WndDesign)