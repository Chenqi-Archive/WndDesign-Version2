#include "ListLayout.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)


struct ListLayoutStyleCalculator : ListLayout::Style {
public:
	uint CalculateGridHeight(uint client_height) const {
		auto& style = GetStyleCalculator(*this);
		LengthStyle height = grid_height;
		height = style.ConvertLengthToPixel(height, client_height);
		height._normal = style.BoundLengthBetween(height._normal, height._min, height._max);
		return height._normal.AsUnsigned();
	}
public:
	uint GetGridLineWidth() const { return gridline._width; }
	Color GetGridLineColor() const { return gridline._color; }
	bool IsGridHeightAuto() const {
		return grid_height._normal.IsAuto();
	}
	const Size CalcualteGridSize(Size client_size) const {
		return Size(client_size.width, CalculateGridHeight(client_size.height));
	}

};

const ListLayoutStyleCalculator& GetStyleCalculator(ListLayout::Style& style) {
	return static_cast<const ListLayoutStyleCalculator&>(style);
}


END_NAMESPACE(Anonymous)


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
	ContentLayoutChanged(row_begin);
}

void ListLayout::EraseEmptyRow(uint row_begin, uint row_count) {
	uint row_number = GetRowNumber();
	if (row_begin >= row_number || row_count == 0) { return; }
	if (row_count > row_number - row_begin) { row_count = row_number - row_begin; }
	_rows.erase(_rows.begin() + row_begin, _rows.begin() + row_begin + row_count);
	ContentLayoutChanged(row_begin);
}

void ListLayout::EraseRow(uint row_begin, uint row_count) {
	if (row_count == 0) { return; }
	RemoveChild(row_begin, row_count);
	EraseEmptyRow(row_begin, row_count);
}

void ListLayout::InsertChild(WndObject& child, uint row) {
	InsertRow(row);
	SetChild(child, row);
}

void ListLayout::SetChild(WndObject& child, uint row) {
	if (row >= GetRowNumber()) { throw std::invalid_argument("invalid row number"); }
	RemoveChild(row);
	RegisterChild(child);
	_rows[row].wnd = &child;
	_rows[row].Invalidate();
	SetChildData(child, row);
}

void ListLayout::RemoveChild(uint row_begin, uint row_count) {
	uint row_number = GetRowNumber();
	if (row_begin >= row_number || row_count == 0) { return; }
	if (row_count > row_number - row_begin) { row_count = row_number - row_begin; }
	for (uint i = row_begin; i < row_begin + row_count; ++i) {
		UnregisterChild(*_rows[i].wnd);
		_rows[i].wnd = nullptr;
	}
	ContentLayoutChanged(row_begin);
}

void ListLayout::OnChildDetach(WndObject& child) {
	uint row = GetChildData(child);
	assert(row < GetRowNumber());
	_rows.
		Rect child_region = child_container.region;
	_child_wnds.erase(child_container.list_index);
}

void ListLayout::ContentLayoutChanged(uint row_begin) {
	if (row_begin < _invalid_layout_row_begin) { _invalid_layout_row_begin = row_begin; }
	Wnd::ContentLayoutChanged();
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
	if (client_size == GetClientSize()) {
	} else {
		if (UpdateDefaultGridSize(style.CalcualteGridSize(client_size))) {
			_invalid_layout_row_begin = 0;
		}
	}
	if (_invalid_layout_row_begin <= GetRowNumber()) {
		Size grid_size = GetDefaultGridSize();
		uint grid_height_max = 
		uint gridline_width = style.GetGridLineWidth();

		uint modified_grid_offset = -1;

		uint y = _rows[_invalid_layout_row_begin].y;

		for (uint i = _invalid_layout_row_begin; i < GetRowNumber(); ++i) {
			uint height = 0;
			if (_rows[i].wnd != nullptr) {
				height = UpdateChildRegion(*_rows[i].wnd, grid_size).size.height;
			}
			BoundHeight();
			if (_rows[i].height != height) {
				if (_rows[i].wnd != nullptr) {
					SetChildRegion(child, Rect(0, y, client_size.width, height));
				}
			}
			_rows[i].y = y;
			_rows[i].height = height;
			y += height + gridline_width;
		}
		Invalidate(Rect(0, , client_size.width, ));
		_invalid_layout_row_begin = -1;
	}
	return Rect(point_zero, Size(client_size.width, GetContentHeight()));
}

void ListLayout::OnChildRegionUpdate(WndObject& child) {
	assert(!GetStyleCalculator().IsGridHeightAuto());
	UpdateChildRegion(child, GetDefaultGridSize());
}

void ListLayout::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {

}


END_NAMESPACE(WndDesign)