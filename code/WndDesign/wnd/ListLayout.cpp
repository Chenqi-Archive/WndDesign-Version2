#include "ListLayout.h"
#include "../style/style_helper.h"

#include <algorithm>


BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)


struct ListLayoutStyleCalculator : ListLayout::Style {
public:
	uint GetGridLineWidth() const { return gridline._width; }
	Color GetGridLineColor() const { return gridline._color; }
	bool IsGridHeightAuto() const { return grid_height._normal.IsAuto(); }
	bool IsGridWidthAuto() const { return client.width._normal.IsAuto(); }
	bool IsGridSizeAuto() const { return IsGridHeightAuto() || IsGridWidthAuto(); }
	uint CalculateGridHeight(uint client_height) const {
		auto& style = GetStyleCalculator(static_cast<const WndStyle&>(*this));
		LengthStyle height = grid_height;
		height = style.ConvertLengthToPixel(height, client_height);
		if (height._normal.IsAuto()) { return height._max.AsUnsigned(); }
		height._normal = style.Clamp(height._normal, height._min, height._max);
		return height._normal.AsUnsigned();
	}
	const std::pair<uint, uint> CalculateMinMaxGridHeight(uint client_height) const {
		LengthStyle height = grid_height;
		height._min.ConvertToPixel(client_height); height._max.ConvertToPixel(client_height);
		if (height._min.AsUnsigned() > height._max.AsUnsigned()) { height._min = height._max; }
		return { height._min.AsUnsigned(), height._max.AsUnsigned() };
	}
};

const ListLayoutStyleCalculator& GetStyleCalculator(ListLayout::Style& style) {
	return static_cast<const ListLayoutStyleCalculator&>(style);
}


END_NAMESPACE(Anonymous)


ListLayout::ListLayout(unique_ptr<Style> style) :
	Wnd(std::move(style)),
	_default_grid_size(),
	_min_max_grid_height(),
	_rows(),
	_content_size(),
	_invalid_layout_row_begin(0) {
}

ListLayout::~ListLayout() {}

uint ListLayout::HitTestRow(uint y) const {
	auto cmp = [](const RowContainer& row, uint y) { return row.y > y; };
	auto it_row = std::lower_bound(_rows.rbegin(), _rows.rend(), y, cmp);
	assert(it_row != _rows.rend());  // underflow (never happens because y >= 0)
	if (it_row == _rows.rbegin() && y - it_row->y >= it_row->height) { return row_end; }  // overflow
	return (uint)(_rows.rend() - it_row - 1);
}

void ListLayout::SetRowNumber(uint row_number) {
	uint current_row_number = GetRowNumber();
	if (row_number > current_row_number) {
		return InsertRow(current_row_number, row_number - current_row_number);
	}
	if (row_number < current_row_number) {
		return EraseRow(row_number, current_row_number - row_number);
	}
}

void ListLayout::InsertRow(uint row_begin, uint row_count) {
	if (row_count == 0) { return; }
	uint row_number = GetRowNumber();
	if (row_begin > row_number) { row_begin = row_number; }
	_rows.insert(_rows.begin() + row_begin, row_count, RowContainer());
	for (uint row = row_begin + row_count; row < row_number + row_count; ++row) {
		if (_rows[row].wnd != nullptr) { SetChildData(*_rows[row].wnd, row); }
	}
	ContentLayoutChanged(row_begin);
}

void ListLayout::EraseEmptyRow(uint row_begin, uint row_count) {
	uint row_number = GetRowNumber();
	if (row_begin >= row_number || row_count == 0) { return; }
	if (row_count > row_number - row_begin) { row_count = row_number - row_begin; }
	_rows.erase(_rows.begin() + row_begin, _rows.begin() + row_begin + row_count);
	for (uint row = row_begin; row < row_number - row_count; ++row) {
		if (_rows[row].wnd != nullptr) { SetChildData(*_rows[row].wnd, row); }
	}
	ContentLayoutChanged(row_begin);
}

void ListLayout::EraseRow(uint row_begin, uint row_count) {
	if (row_count == 0) { return; }
	RemoveChild(row_begin, row_count);
	EraseEmptyRow(row_begin, row_count);
}

void ListLayout::SetChild(WndObject& child, uint row) {
	if (row >= GetRowNumber()) { throw std::invalid_argument("invalid row number"); }
	RemoveChild(row);
	RegisterChild(child);
	_rows[row].wnd = &child;
	_rows[row].Invalidate();
	SetChildData(child, row);
}

void ListLayout::InsertChild(WndObject& child, uint row) {
	uint row_number = GetRowNumber();
	if (row > row_number) { row = row_number; }
	InsertRow(row);
	SetChild(child, row);
}

void ListLayout::RemoveChild(uint row_begin, uint row_count) {
	uint row_number = GetRowNumber();
	if (row_begin >= row_number || row_count == 0) { return; }
	if (row_count > row_number - row_begin) { row_count = row_number - row_begin; }
	for (uint row = row_begin; row < row_begin + row_count; ++row) {
		if (_rows[row].wnd != nullptr) {
			WndObject::RemoveChild(*_rows[row].wnd);
		}
	}
}

void ListLayout::OnChildDetach(WndObject& child) {
	Wnd::OnChildDetach(child);
	uint row = GetChildData(child);
	assert(row < GetRowNumber() && _rows[row].wnd == &child);
	_rows[row].wnd = nullptr;
	_rows[row].Invalidate();
	ContentLayoutChanged(row);
}

void ListLayout::ContentLayoutChanged(uint row_begin) {
	if (_invalid_layout_row_begin == -1) { Wnd::ContentLayoutChanged(); }
	if (row_begin < _invalid_layout_row_begin) { _invalid_layout_row_begin = row_begin; }
}

void ListLayout::ChildRegionMayChange(WndObject& child) {
	if (GetStyleCalculator(GetStyle()).IsGridSizeAuto()) {
		uint row = GetChildData(child);
		_rows[row].Invalidate();
		ContentLayoutChanged(row);
	}
}

const Rect ListLayout::UpdateContentLayout(Size client_size) {
	auto& style = GetStyleCalculator(GetStyle());
	bool all_invalid = false;
	if (client_size != GetClientSize()) {
		bool default_grid_size_changed = UpdateDefaultGridSize(Size(client_size.width, style.CalculateGridHeight(client_size.height)));
		bool min_max_grid_height_changed = UpdateMinMaxGridHeight(style.CalculateMinMaxGridHeight(client_size.height));
		if (default_grid_size_changed || min_max_grid_height_changed) {
			_invalid_layout_row_begin = 0;
			all_invalid = true;
		}
	}
	if (_invalid_layout_row_begin >= GetRowNumber()) {
		return Rect(point_zero, _content_size);
	}
	uint min_grid_height = _min_max_grid_height.first, max_grid_height = _min_max_grid_height.second;
	uint y0 = _rows[_invalid_layout_row_begin].y;
	uint y = y0;
	uint gridline_width = style.GetGridLineWidth();
	uint max_child_width = 0;
	for (uint row = 0; row < _invalid_layout_row_begin; ++row) {
		max_child_width = std::max(max_child_width, _rows[row].width);
	}
	for (uint row = _invalid_layout_row_begin; row < GetRowNumber(); ++row) {
		RowContainer& row_container = _rows[row];
		uint height = row_container.height;
		uint width = row_container.width;
		if (all_invalid || row_container.IsInvalid()) {
			if (row_container.wnd == nullptr) {
				height = min_grid_height;
				width = 0;
			} else {
				Size size = UpdateChildRegion(*row_container.wnd, _default_grid_size).size;
				height = StyleCalculator::Clamp(size.height, min_grid_height, max_grid_height);
				width = std::min(size.width, client_size.width);
			}
		}
		max_child_width = std::max(max_child_width, width);
		assert(max_child_width <= client_size.width);
		row_container.y = y;
		row_container.height = height;
		row_container.width = width;
		if (row_container.wnd != nullptr) {
			SetChildRegion(*row_container.wnd, Rect(0, (int)y, width, height));
		}
		y += height + gridline_width;
	}
	Invalidate(Rect(0, (int)y0, std::max(_content_size.width, max_child_width), std::max(_content_size.height, y) - y0));
	_content_size.width = max_child_width;
	_content_size.height = y > gridline_width ? y - gridline_width : 0;
	_invalid_layout_row_begin = -1;
	return Rect(point_zero, _content_size);
}

void ListLayout::OnChildRegionUpdate(WndObject& child) {
	uint row = GetChildData(child);
	assert(row < GetRowNumber());
	assert(!GetStyleCalculator(GetStyle()).IsGridSizeAuto());
	Size size = UpdateChildRegion(child, _default_grid_size).size;
	_rows[row].width = std::min(size.width, _default_grid_size.width);
	SetChildRegion(child, Rect(0, _rows[row].y, _rows[row].width, std::min(size.height, _rows[row].height)));
}

void ListLayout::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {
	uint y1 = (uint)invalid_client_region.top(), y2 = (uint)invalid_client_region.bottom() - 1;
	uint row1 = HitTestRow(y1), row2 = HitTestRow(y2);
	if (row1 == row_end) { return; } 
	if (row2 == row_end) { row2 = GetRowNumber() - 1; }
	assert(row1 != row_end && row2 != row_end && row2 >= row1);
	for (uint row = row1; row <= row2; ++row) {
		const RowContainer& row_container = _rows[row];
		// Composite child window.
		if (row_container.wnd != nullptr) {
			Rect child_region = Rect(0, (int)row_container.y, row_container.width, row_container.height);
			Rect child_invalid_region = child_region.Intersect(invalid_client_region);
			if (!child_invalid_region.IsEmpty()) {
				CompositeChild(*row_container.wnd, figure_queue, child_invalid_region);
			}
		}
		// Draw grid line as rectangle.
		figure_queue.Append(
			Point(0, row_container.y + row_container.height),
			new Rectangle(Size(client_region.size.width, GetStyle().gridline._width), GetStyle().gridline._color)
		);
	}
}

const Wnd::HitTestInfo ListLayout::ClientHitTest(Size client_size, Point point) const {
	uint y = (uint)point.y;	uint row = HitTestRow(y);
	if (row == row_end || _rows[row].wnd == nullptr || y - _rows[row].y >= _rows[row].height) { return {}; }
	return { _rows[row].wnd, Point(point.x, y - _rows[row].y) };
}


END_NAMESPACE(WndDesign)