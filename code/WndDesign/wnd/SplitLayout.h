#pragma once

#include "Wnd.h"


BEGIN_NAMESPACE(WndDesign)


// vertical split layout
class SplitLayout : public Wnd {
public:
	struct Style : Wnd::Style {
		struct SplitLineStyle {
		public:
			uint _width = 3;
			Color _color = ColorSet::Black;
			ValueTag _position = 50pct;  // absolute value is relative to left side
		public:
			constexpr SplitLineStyle& width(uint width) { _width = width; return *this; }
			constexpr SplitLineStyle& color(Color color) { _color = color; return *this; }
			constexpr SplitLineStyle& position(ValueTag position) { _position = position; return *this; }
		}split_line;
	};

public:
	SplitLayout(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~SplitLayout() {}


	//// style ////
protected:
	Style& GetStyle() { return static_cast<Style&>(Wnd::GetStyle()); }
	const Style& GetStyle() const { return static_cast<const Style&>(Wnd::GetStyle()); }


	//// child windows ////
private:
	ref_ptr<WndObject> _child_left = nullptr;
	ref_ptr<WndObject> _child_right = nullptr;

private:
	static void SetChildData(WndObject& child, bool is_left) { WndObject::SetChildData<bool>(child, is_left); }
	static bool GetChildData(WndObject& child) { return WndObject::GetChildData<bool>(child); }
public:
	void SetChildLeft(WndObject& child);
	void SetChildRight(WndObject& child);
	void RemoveChildLeft();
	void RemoveChildRight();
private:
	virtual void OnChildDetach(WndObject& child) override;


	//// layout update ////
private:
	Rect _region_left;
	Rect _region_right;
	Rect _region_split_line;

private:
	void UpdateChildRegion(WndObject& child, bool is_left);
private:
	virtual void OnChildRegionUpdate(WndObject& child) override;
	virtual const Rect UpdateContentLayout(Size client_size) override;


	//// painting and composition ////
private:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const;


	//// message handling ////
private:
	enum class MousePosition : uchar { None, Other, SplitLine } _mouse_position = MousePosition::None;
	bool _is_mouse_dragging = false;
	bool _is_layout_invalid = false;
	int _mouse_down_position = 0;
	int _mouse_down_split_line_position = 0;
private:
	virtual const Wnd::HitTestInfo ClientHitTest(Size client_size, Point point) const override;
	virtual void Handler(Msg msg, Para para) override;
};


END_NAMESPACE(WndDesign)