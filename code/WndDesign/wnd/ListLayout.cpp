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
	uint CalculateGridHeight(uint client_height) const {
		auto& style = GetStyleCalculator(static_cast<const WndStyle&>(*this));
		LengthStyle height = grid_height;
		height = style.ConvertLengthToPixel(height, client_height);
		if (height._normal.IsAuto()) { return height._max.AsUnsigned(); }
		height._normal = style.BoundLengthBetween(height._normal, height._min, height._max);
		return height._normal.AsUnsigned();
	}
	std::pair<uint, uint> CalculateMinMaxGridHeight(uint client_height) const {
		LengthStyle height = grid_height;
		height._min.ConvertToPixel(client_height); height._max.ConvertToPixel(client_height);
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
	_rows(),
	_invalid_layout_row_begin(0)
{
}

ListLayout::~ListLayout() {}

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
	InsertRow(row);
	SetChild(child, row);
}

void ListLayout::RemoveChild(uint row_begin, uint row_count) {
	uint row_number = GetRowNumber();
	if (row_begin >= row_number || row_count == 0) { return; }
	if (row_count > row_number - row_begin) { row_count = row_number - row_begin; }
	for (uint row = row_begin; row < row_begin + row_count; ++row) {
		UnregisterChild(*_rows[row].wnd);
		_rows[row].wnd = nullptr;
		_rows[row].Invalidate();
	}
	ContentLayoutChanged(row_begin);
}

void ListLayout::OnChildDetach(WndObject& child) {
	Wnd::OnChildDetach(child);
	uint row = GetChildData(child);
	assert(row < GetRowNumber());
	_rows[row].wnd = nullptr;
	_rows[row].Invalidate();
	ContentLayoutChanged(row);
}

void ListLayout::ContentLayoutChanged(uint row_begin) {
	if (_invalid_layout_row_begin == -1) { Wnd::ContentLayoutChanged(); }
	if (row_begin < _invalid_layout_row_begin) { _invalid_layout_row_begin = row_begin; }
}

uint ListLayout::GetContentHeight() const {
	uint row_number = GetRowNumber();
	return row_number == 0 ? 0 : _rows[row_number - 1].y + _rows[row_number - 1].height;
}

void ListLayout::ChildRegionMayChange(WndObject& child) {
	if (GetStyleCalculator(GetStyle()).IsGridHeightAuto()) {
		uint row = GetChildData(child);
		_rows[row].Invalidate();
		ContentLayoutChanged();
	}
}

const Rect ListLayout::UpdateContentLayout(Size client_size) {
	auto& style = GetStyleCalculator(GetStyle());
	bool all_invalid = false;
	if (client_size != GetClientSize()) {
		if (UpdateDefaultGridSize(Size(style.CalculateGridHeight(client_size.height), client_size.width))) {
			_invalid_layout_row_begin = 0;
			all_invalid = true;
		}
	}
	if (_invalid_layout_row_begin >= GetRowNumber()) {
		return Rect(point_zero, Size(client_size.width, GetContentHeight()));
	}
	auto [min_grid_height, max_grid_height] = style.CalculateMinMaxGridHeight(client_size.height);
	auto BoundHeightBetween = [](uint height, uint min_height, uint max_height) -> uint {
		if (height < min_height) { height = min_height; } if (height > max_height) { height = max_height; } return height;
	};
	uint y0 = _rows[_invalid_layout_row_begin].y;
	uint y = y0;
	uint gridline_width = style.GetGridLineWidth();
	for (uint row = _invalid_layout_row_begin; row < GetRowNumber(); ++row) {
		uint height = 0;
		if (_rows[row].wnd != nullptr && (all_invalid || _rows[row].IsInvalid())) {
			height = UpdateChildRegion(*_rows[row].wnd, _default_grid_size).size.height;
		}
		height = BoundHeightBetween(height, min_grid_height, max_grid_height);
		_rows[row].y = y;
		_rows[row].height = height;
		if (_rows[row].wnd != nullptr) {
			SetChildRegion(*_rows[row].wnd, Rect(0, (int)y, client_size.width, height));
		}
		y += height + gridline_width;
	}
	uint height = GetContentHeight();
	Invalidate(Rect(0, (int)y0, client_size.width, height - y0));
	_invalid_layout_row_begin = -1;
	return Rect(point_zero, Size(client_size.width, height));
}

void ListLayout::OnChildRegionUpdate(WndObject& child) {
	assert(!GetStyleCalculator(GetStyle()).IsGridHeightAuto());
	UpdateChildRegion(child, GetDefaultGridSize());
}

void ListLayout::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {

}

const Wnd::HitTestInfo ListLayout::ClientHitTest(Size client_size, Point point) const {
	return Wnd::HitTestInfo();
}


END_NAMESPACE(WndDesign)