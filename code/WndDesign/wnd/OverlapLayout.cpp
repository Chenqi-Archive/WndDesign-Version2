#include "OverlapLayout.h"
#include "../common/reversion_wrapper.h"


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
		SetChildRegion(child, child_region); // UpdateChildRegion must be followed by SetChildRegion
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
				SetChildRegion(child_container.wnd, child_region);
			}
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

bool OverlapLayout::ClientHandler(Msg msg, Para para) {
	if (IsMouseMsg(msg)) {
		MouseMsg mouse_msg = GetMouseMsg(para);
		ref_ptr<WndObject> child = nullptr;
		Point point; 
		for (auto& child_container : _child_wnds) {
			point = mouse_msg.point;
			if (child_container.region.Contains(point)) {
				point = point - (child_container.region.point - point_zero);
				if (HitTestChild(child_container.wnd, point)) {
					child = &child_container.wnd;
					break;
				}
			}
		}
		if (child != nullptr) {
			mouse_msg.point = point;
			if (SendChildMessage(*child, msg, mouse_msg)) {
				return true;
			}
		}
	}
	return false; 
}


END_NAMESPACE(WndDesign)