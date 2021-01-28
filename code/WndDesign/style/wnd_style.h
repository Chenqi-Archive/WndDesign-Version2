#pragma once

#include "value_tag.h"
#include "../figure/color.h"
#include "../figure/background_types.h"


BEGIN_NAMESPACE(WndDesign)


struct WndStyle {

	////
	//// styles related with region on parent window
	//// 
	//// (size and position determines the display region of the window on parent window)

	// The size of the window (may be in fixed size or relative to parent window).
	struct LengthStyle {
	public:
		ValueTag _normal = length_auto;
		ValueTag _min = px(length_min);
		ValueTag _max = px(length_max);
		constexpr LengthStyle& normal(ValueTag normal) { _normal = normal; return *this; }
		constexpr LengthStyle& min(ValueTag min) { _min = min; return *this; }
		constexpr LengthStyle& max(ValueTag max) { _max = max; return *this; }
		constexpr void setFixed(ValueTag length) { _normal = _min = _max = length; }
	};
	LengthStyle width;
	LengthStyle height;


	// The position of the window, or the margin between the window and the parent window's client region.
	// (relative to the inner edges of parent window)
	struct PositionStyle {
	public:
		ValueTag _left = position_auto;
		ValueTag _top = position_auto;
		ValueTag _right = position_auto;
		ValueTag _down = position_auto;
	public:
		constexpr PositionStyle& left(ValueTag left) { _left = left; return *this; }
		constexpr PositionStyle& top(ValueTag top) { _top = top; return *this; }
		constexpr PositionStyle& right(ValueTag right) { _right = right; return *this; }
		constexpr PositionStyle& down(ValueTag down) { _down = down; return *this; }
		constexpr void set(ValueTag left, ValueTag top, ValueTag right, ValueTag down) { _left = left; _top = top; _right = right; _down = down; }
		constexpr void setAll(ValueTag all) { _left = _top = _right = _down = all; }
		constexpr PositionStyle& setHorizontalCenter() { _left = _right = position_center; return *this; }
		constexpr PositionStyle& setVerticalCenter() { _top = _down = position_center; return *this; }
	}position;


	////
	//// styles related with client region
	//// 
	//// (border, padding and scroll bar determines the client region that is relative to the region on parent window)
	//// (client region is maintained by WndFrame and is not aware by the inner window layout)

	// The border of the window.
	struct BorderStyle {
	public:
		float _width = 0;
		Color _color = ColorSet::Black;
		uint _radius = 0;
	public:
		constexpr BorderStyle& width(float width) { _width = width; return *this; }
		constexpr BorderStyle& color(Color color) { _color = color; return *this; }
		constexpr BorderStyle& radius(Color color) { _color = color; return *this; }
	}border;


	// The padding of the window.
	// (calculated relative to the size of parent window, and is extended from the inner side of the border)
#error Wait, the padding should be extended from the accessible region, also remind of the background
	struct PaddingStyle {
	public:
		ValueTag _left = 0px;
		ValueTag _top = 0px;
		ValueTag _right = 0px;
		ValueTag _down = 0px;
	public:
		constexpr PaddingStyle& left(ValueTag left) { _left = left; return *this; }
		constexpr PaddingStyle& top(ValueTag top) { _top = top; return *this; }
		constexpr PaddingStyle& right(ValueTag right) { _right = right; return *this; }
		constexpr PaddingStyle& down(ValueTag down) { _down = down; return *this; }
	}padding;


	// The scroll bar can be customized by user.


	////
	//// styles related with accessible region
	//// 
	//// (region can be relative to client region and determine the size of child windows, or auto fit the size 
	////    of child windows or contents, and the actual accessible region must be larger than client region)
	//// (if the region overflows the client region, scroll bar can be drawn)

	// The accessible region of the window.
	struct RegionStyle {
	public:
		ValueTag _left = 0px;
		ValueTag _top = 0px;
		ValueTag _right = 0px;
		ValueTag _down = 0px;
		bool _vertical_scroll_bar = true;
		bool _horizontal_scroll_bar = true;
	public:
		constexpr void set(ValueTag left, ValueTag top, ValueTag right, ValueTag down) { _left = left; _top = top; _right = right; _down = down; }
		constexpr void setInfinite() { set(position_min_tag, position_min_tag, position_max_tag, position_max_tag); }
		constexpr RegionStyle& vertical_scroll_bar(bool vertical_scroll_bar = true) { _vertical_scroll_bar = vertical_scroll_bar; return *this; }
		constexpr RegionStyle& horizontal_scroll_bar(bool horizontal_scroll_bar = true) { _horizontal_scroll_bar = horizontal_scroll_bar; return *this; }
	}region;


	////
	//// other window styles
	//// 

	// The background of the window.
	struct BackgroundStyle {
	public:
		shared_ptr<Background> _background_resource = GetNullBackground();
	public:
		void setColor(Color color) { _background_resource.reset(new SolidColorBackground(color)); }
		void setImage(const wstring& image_file, uchar opacity = 0xFF, Vector offset_on_image = vector_zero) {
			_background_resource.reset(new ImageRepeatBackground(Image(image_file), opacity, offset_on_image));
		}
		void setImage(const Image& image, uchar opacity = 0xFF, Vector offset_on_image = vector_zero) {
			_background_resource.reset(new ImageRepeatBackground(image, opacity, offset_on_image));
		}
		const Background& get() const { return *_background_resource; }
	}background;

#error style might contain allocated resources like background, use virutal destructor!
};


END_NAMESPACE(WndDesign)