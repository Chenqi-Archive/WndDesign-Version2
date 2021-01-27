#pragma once

#include "value_tag.h"
#include "../figure/color.h"
#include "../figure/background_types.h"


BEGIN_NAMESPACE(WndDesign)


struct WndStyle {

	// The size of the window (may be in fixed size or relative to parent window).
	struct LengthStyle {
	public:
		ValueTag _normal = length_auto;
		ValueTag _min = px(length_min);
		ValueTag _max = px(length_max);
		constexpr void setFixed(ValueTag length) { _normal = _min = _max = length; }
		constexpr LengthStyle& normal(ValueTag normal) { _normal = normal; return *this; }
		constexpr LengthStyle& min(ValueTag min) { _min = min; return *this; }
		constexpr LengthStyle& max(ValueTag max) { _max = max; return *this; }
	};
	LengthStyle width;
	LengthStyle height;


	// The position of the window, or the margin between the window and the parent window's client region.
	// Size and position determines the display region of the window on parent window. 
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
		uint _radius = 0;
	public:
		constexpr BorderStyle& width(float width) { _width = width; return *this; }
		constexpr BorderStyle& color(Color color) { _color = color; return *this; }
		constexpr BorderStyle& radius(Color color) { _color = color; return *this; }
	}border;


	// The accessible region of the window, relative to the window's display size, 
	//   and is the same as the display size by default.
	// The region might auto fit child windows, or be used for calculating child windows' size.
	struct RegionStyle {
	public:
		ValueTag _left = 0pct;
		ValueTag _top = 0pct;
		ValueTag _width = 100pct;
		ValueTag _height = 100pct;
		bool _width_auto_resize = false;
		bool _height_auto_resize = false;
	#error may combine the auto resize style into ValueTag
	#error may also contain the min and max region config.
	public:
		constexpr void set(ValueTag left, ValueTag top, ValueTag width, ValueTag height) {
			_left = left; _top = top; _width = width; _height = height;
		}
		constexpr void setInfinite() { set(position_min_tag, position_min_tag, length_max_tag, length_max_tag); }
		constexpr RegionStyle& width_auto_resize(bool width_auto_resize = true) { _width_auto_resize = width_auto_resize; return *this; }
		constexpr RegionStyle& height_auto_resize(bool height_auto_resize = true) { _height_auto_resize = height_auto_resize; return *this; }
	}region;


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
};


END_NAMESPACE(WndDesign)