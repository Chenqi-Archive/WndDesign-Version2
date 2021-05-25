#include "FlowLayout.h"

#include <algorithm>


BEGIN_NAMESPACE(WndDesign)


uint FlowLayout::HitTestChild(uint x, uint y) const {
	// hit test row
	if (_rows.empty()) { return pos_end; }
	auto cmp_row = [](const RowContainer& row, uint y) { return row.y <= y; };
	auto it_row = std::lower_bound(_rows.begin(), _rows.end(), y, cmp_row);
	assert(it_row != _rows.begin()); it_row--;
	if (y - it_row->y >= it_row->height) { return pos_end; }
	// hit test child of the row
	auto cmp_pos = [](const ChildContainer& child, uint x) { return child.x <= x; };
	auto it_child = std::lower_bound(_child_wnds.begin() + it_row->child_begin, _child_wnds.begin() + it_row->child_end, x, cmp_pos);
	assert(it_child != _child_wnds.begin() + it_row->child_begin); it_child--;
	if (x - it_child->x >= it_child->size.width) { return pos_end; }
	return (uint)(it_child - _child_wnds.begin());
}

void FlowLayout::SetChild(WndObject& child, uint pos) {
	if (pos >= _child_wnds.size()) { throw std::invalid_argument("invalid row number"); }
	RemoveChild(pos);
	RegisterChild(child);
	_rows[row].wnd = &child;
	_rows[row].Invalidate();
	SetChildData(child, row);
}

void FlowLayout::InsertChild(WndObject& child, uint pos) {
}

void FlowLayout::RemoveChild(uint pos_begin, uint child_count) {
}

void FlowLayout::EraseChild(uint pos_begin, uint child_count) {
}

void FlowLayout::OnChildDetach(WndObject& child) {
}

void FlowLayout::ContentLayoutChanged(uint row_begin) {

}

void FlowLayout::ChildRegionMayChange(WndObject& child) {

}

const Rect FlowLayout::UpdateContentLayout(Size client_size) {

}

void FlowLayout::OnChildRegionUpdate(WndObject& child) {

}

void FlowLayout::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {

}

const Wnd::HitTestInfo FlowLayout::ClientHitTest(Size client_size, Point point) const {

}


END_NAMESPACE(WndDesign)