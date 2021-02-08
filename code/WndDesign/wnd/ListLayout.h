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
	~ListLayout() {}


	//// style ////
protected:
	Style& GetStyle() { return static_cast<Style&>(Wnd::GetStyle()); }
	const Style& GetStyle() const { return static_cast<const Style&>(Wnd::GetStyle()); }

private:
	Size _default_grid_size;
	uint _min_grid_height, _max_grid_height;
private:
	const Size GetDefaultGridSize() { return _default_grid_size; }
	bool UpdateDefaultGridSize(Size grid_size, uint min_grid_height, uint max_grid_height) {

		return _default_grid_size == grid_size ? false : (_default_grid_size = grid_size, true);
	}


	//// row operation ////
private:
	struct RowContainer {
		uint y = 0;
		uint height = 0;
		ref_ptr<WndObject> wnd = nullptr;
	public:
		void Invalidate() { height = -1; }
		bool IsInvalid() const { return height == -1; }
	};
	vector<RowContainer> _rows;

public:
	constexpr static uint row_end = -1;

public:
	uint GetRowNumber() const { return (uint)_rows.size(); }
	void SetRowNumber(uint row_number);
	void InsertRow(uint row_begin, uint row_count = 1);
private:
	void EraseEmptyRow(uint row_begin, uint row_count);
public:
	void EraseRow(uint row_begin, uint row_count = 1);


	//// child windows ////
private:
	static void SetChildData(WndObject& child, uint row) {
		WndObject::SetChildData(child, static_cast<ulonglong>(row));
	}
	static uint GetChildData(WndObject& child) {
		return WndObject::GetChildData<ulonglong>(child);
	}

public:
	void SetChild(WndObject& child, uint row);
	void RemoveChild(uint row_begin, uint row_count = 1);
	void InsertChild(WndObject& child, uint row = row_end);
	void AppendChild(WndObject& child) { InsertChild(child); }
	void EraseChild(uint row_begin, uint row_count = 1) { EraseRow(row_begin, row_count); }

private:
	virtual void OnChildDetach(WndObject& child) override;


	//// layout update ////
private:
	uint _invalid_layout_row_begin = -1;
private:
	void ContentLayoutChanged(uint row_begin = 0);
	uint GetContentHeight() const;
private:
	virtual void ChildRegionMayChange(WndObject& child) override;
	virtual const Rect UpdateContentLayout(Size client_size);
	virtual void OnChildRegionUpdate(WndObject& child) override;


	//// painting and composition ////
private:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const;


	//// message handling ////
private:
	struct HitTestInfo {

	};

	const HitTestInfo HitTestPoint(Point point) {

	}
};


END_NAMESPACE(WndDesign)