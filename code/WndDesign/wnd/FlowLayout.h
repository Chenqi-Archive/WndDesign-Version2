#pragma once

#include "Wnd.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


// left to right, top to bottom flow layout
class FlowLayout : public Wnd {
public:
	struct Style : public Wnd::Style {
		struct SpacingStyle {
		public:
			uint _item = 5;
			uint _line = 5;
		public:
			constexpr SpacingStyle& item(uint item) { _item = item; return *this; }
			constexpr SpacingStyle& line(uint line) { _line = line; return *this; }
		}spacing;

		Style() {
			scrollbar.set(CreateDefaultScrollbar());
			client.height.min(0px).normal(length_auto).max(length_max_tag);
			client.width.min(0px).normal(length_auto).max(100pct);
		}
	};

public:
	FlowLayout(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~FlowLayout() {}


	//// style ////
protected:
	Style& GetStyle() { return static_cast<Style&>(Wnd::GetStyle()); }
	const Style& GetStyle() const { return static_cast<const Style&>(Wnd::GetStyle()); }


	//// child windows and row management ////
public:
	static constexpr uint pos_end = -1;

private:
	struct ChildContainer {
		ref_ptr<WndObject> wnd = nullptr;
		Size size = size_empty;
		uint row = -1;
		uint x = -1;
	};

	struct RowContainer {
		uint y = 0;
		uint height = 0;
		uint width = 0;
		uint child_begin = pos_end;
		uint child_end = pos_end;
	};

private:
	vector<ChildContainer> _child_wnds;
	vector<RowContainer> _rows;

private:
	uint HitTestChild(uint x, uint y) const;

private:
	static void SetChildData(WndObject& child, uint row) { WndObject::SetChildData<uint>(child, row); }
	static uint GetChildData(WndObject& child) { return WndObject::GetChildData<uint>(child); }
public:
	void SetChild(WndObject& child, uint pos);
	void InsertChild(WndObject& child, uint pos);
	void AppendChild(WndObject& child) { InsertChild(child, pos_end); }
	void RemoveChild(uint pos_begin, uint child_count = 1);
	void EraseChild(uint pos_begin, uint child_count = 1);
private:
	virtual void OnChildDetach(WndObject& child) override;


	//// layout update ////
private:
	uint _invalid_layout_child_begin = 0;
private:
	void ContentLayoutChanged(uint row_begin);
private:
	virtual void ChildRegionMayChange(WndObject& child) override;
	virtual const Rect UpdateContentLayout(Size client_size);
	virtual void OnChildRegionUpdate(WndObject& child) override;


	//// painting and composition ////
private:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const;


	//// message handling ////
private:
	virtual const Wnd::HitTestInfo ClientHitTest(Size client_size, Point point) const override;
};


END_NAMESPACE(WndDesign)