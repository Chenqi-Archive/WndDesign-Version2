#pragma once

#include "Wnd.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


class ListLayout : public Wnd {
public:
	struct Style : public Wnd::Style {
		struct GridlineStyle {
			uint _width = 3;
			Color _color = ColorSet::Black;
		public:
			constexpr GridlineStyle& width(uint width) { _width = width; return *this; }
			constexpr GridlineStyle& color(Color color) { _color = color; return *this; }
		}gridline;

		LengthStyle grid_height;
	};

public:
	ListLayout(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~ListLayout();


	//// row operation ////
private:
	struct RowContainer {
		uint y = 0;
		uint height = 0;
		ref_ptr<WndObject> wnd = nullptr;
	};
	vector<RowContainer> _rows;

public:
	constexpr static uint row_end = -1;

private:
	void EraseEmptyRows(uint row_begin, uint row_count) {
		_rows.erase(_rows.begin() + row_begin, _rows.begin() + row_begin + row_count);
	}
public:
	uint GetRowNumber() const { return (uint)_rows.size(); }
	void SetRowNumber(uint row_number) {
		uint current_row_number = GetRowNumber();
		if (row_number > current_row_number) {
			return InsertRow(current_row_number, row_number - current_row_number);
		}
		if (row_number < current_row_number) {
			return DeleteRow(row_number, current_row_number - row_number);
		}
		// if (row_number == current_row_number) { return; }
	}
	void InsertRow(uint row_begin, uint row_count = 1) {
		if (row_count == 0) { return; }
		_rows.insert(_rows.begin() + row_begin, row_count, RowContainer());
		ChildLayoutChanged();
	}
	void DeleteRow(uint row_begin, uint row_count = 1) {
		if (row_count == 0) { return; }
		RemoveChild(row_begin, row_count);
		EraseEmptyRows(row_begin, row_count);
	}


	//// child windows ////
private:
	static void SetChildData(WndObject& child, uint row) {
		WndObject::SetChildData(child, static_cast<ulonglong>(row));
	}
	static uint GetChildData(WndObject& child) {
		return WndObject::GetChildData<ulonglong>(child);
	}
public:
	void SetChild(WndObject& child, uint row) {
		RegisterChild(child);
		if (row >= GetRowNumber()) { return AppendChild(child); }
		RemoveChild(row);
		_rows[row].wnd = &child;
		SetChildData(child, row);
		ChildLayoutChanged();
	}
	void AddChild(WndObject& child, uint row = row_end) {
		InsertRow(row);
		SetChild(child, row);
	}
	void AppendChild(WndObject& child) { AddChild(child); }
	void RemoveChild(uint row_begin, uint row_count = 1) {


		ChildLayoutChanged();
	}
	virtual void OnChildDetach(WndObject& child) override {
		uint row = GetChildData(child);
		assert(row < GetRowNumber());
		_rows.
		Rect child_region = child_container.region;
		_child_wnds.erase(child_container.list_index);
	}


	//// message handling ////
private:
	struct HitTestInfo {

	};

	const HitTestInfo HitTestPoint(Point point) {

	}
};


END_NAMESPACE(WndDesign)