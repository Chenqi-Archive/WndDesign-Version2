#pragma once

#include "value_tag.h"
#include "../figure/color.h"
#include "../figure/background_types.h"


BEGIN_NAMESPACE(WndDesign)


struct WndStyle {

	// The size of the window (may be fixed or relative to parent window).
	struct SizeStyle {
	public:
		struct SizeTag { ValueTag width; ValueTag height; };
		SizeTag _normal = { length_auto, length_auto };
		SizeTag _min = { px(length_min), px(length_min) };
		SizeTag _max = { px(length_max), px(length_max) };
	public:
		constexpr SizeStyle& normal(ValueTag width, ValueTag height) { _normal = { width, height }; return *this; }
		constexpr SizeStyle& min(ValueTag width, ValueTag height) { _min = { width, height }; return *this; }
		constexpr SizeStyle& max(ValueTag width, ValueTag height) { _max = { width, height }; return *this; }
		constexpr void setFixed(ValueTag width, ValueTag height) { _normal = _min = _max = { width, height }; }
	}size;


	// The position of the window, or the margin between the window and the parent window's client region.
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
		constexpr PositionStyle& setHorizontalCenter() { _left = _right = position_center; return *this; }
		constexpr PositionStyle& setVerticalCenter() { _top = _down = position_center; return *this; }
	}position;


	// The border of the window.
	struct BorderStyle {
	public:
		float _width = 0;
		Color _color = ColorSet::Black;
	public:
		constexpr BorderStyle& width(float width) { _width = width; return *this; }
		constexpr BorderStyle& color(Color color) { _color = color; return *this; }
	}border;


	// The background of the window.
	struct BackgroundStyle {
	public:
		shared_ptr<Background> _background_resource;
	public:
		void setColor(Color color) { _background_resource.reset(new SolidColorBackground(color)); }
		void setImage(const wstring& image_file, uchar opacity = 0xFF, Vector offset_on_image = vector_zero) {
			_background_resource.reset(new ImageRepeatBackground(Image(image_file), opacity, offset_on_image));
		}
		void setImage(const Image& image, uchar opacity = 0xFF, Vector offset_on_image = vector_zero) {
			_background_resource.reset(new ImageRepeatBackground(image, opacity, offset_on_image));
		}
	}background;
};


END_NAMESPACE(WndDesign)