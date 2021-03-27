#pragma once

#include "wnd_style.h"
#include "../geometry/interval.h"
#include "../geometry/geometry_helper.h"
#include "../figure/figure_types.h"


BEGIN_NAMESPACE(WndDesign)


// a helper class for calculating region from style
struct StyleCalculator : public WndStyle {
private:
	StyleCalculator() = delete;

public:
	static inline const std::invalid_argument style_parse_exception = std::invalid_argument("style parse error");

	// style dependency identification
public:
	static bool IsLengthRelative(const LengthStyle& length) {
		return length._normal.IsPercent() || length._min.IsPercent() || length._max.IsPercent();
	}
	static bool IsLengthAuto(const LengthStyle& length) {
		return length._normal.IsAuto(); /* && length._min != length._max */
	}
	static bool IsPositionRelative(const PositionStyle& position) {
		return position._left.IsPercent() || position._top.IsPercent() || position._right.IsPercent() || position._bottom.IsPercent();
	}
	static bool IsPositionAuto(ValueTag position) {
		return position.IsAuto() || position.IsCenter();
	}
	static bool IsPaddingRelative(const PaddingStyle& padding) {
		return padding._left.IsPercent() || padding._top.IsPercent() || padding._right.IsPercent() || padding._bottom.IsPercent();
	}
	static bool IsScrollbarAuto(const ScrollbarStyle& scrollbar) {
		return scrollbar._resource->IsMarginAuto();
	}
	static bool IsClientRelative(const ClientStyle& client) {
		return client._left.IsPercent() || client._top.IsPercent() || IsLengthRelative(client.width) || IsLengthRelative(client.height);
	}
	static bool IsClientAuto(const ClientStyle& client) {
		return IsLengthAuto(client.width) || IsLengthAuto(client.height);
	}
public:
	bool IsRegionHorizontalAuto() const {
		return IsLengthAuto(width) && (IsPositionAuto(position._left) || IsPositionAuto(position._right));
	}
	bool IsRegionVerticalAuto() const {
		return IsLengthAuto(height) && (IsPositionAuto(position._top) || IsPositionAuto(position._bottom));
	}
public:
	bool IsRegionOnParentRelative() const {
		return IsLengthRelative(width) || IsLengthRelative(height) || IsPositionRelative(position);
	}
	bool IsRegionOnParentAuto() const {
		return IsRegionHorizontalAuto() || IsRegionVerticalAuto();
	}
	bool IsMarginRelative() const {
		return IsPaddingRelative(padding);
	}
	bool IsMarginAuto() const {
		return IsScrollbarAuto(scrollbar);
	}
	bool IsClientRegionRelative() const {
		return IsClientRelative(client);
	}
	bool IsClientRegionAuto() const {
		return IsClientAuto(client);
	}

	// region calculation
public:
	static const Size CalculateSizeFromTag(ValueTag width, ValueTag height, Size parent_size) {
		width.ConvertToPixel(parent_size.width); height.ConvertToPixel(parent_size.height);
		return Size(width.AsUnsigned(), height.AsUnsigned());
	}
public:
	const std::pair<Size, Size> CalculateMinMaxDisplaySize(Size parent_size) const {
		return { CalculateSizeFromTag(width._min, height._min, parent_size), CalculateSizeFromTag(width._max, height._max, parent_size) };
	}
public:
	static const LengthStyle ConvertLengthToPixel(LengthStyle length, uint parent_length) {
		length._normal.ConvertToPixel(parent_length);
		length._min.ConvertToPixel(parent_length);
		length._max.ConvertToPixel(parent_length);
		return length;
	}
	static uint Clamp(uint normal_length, uint min_length, uint max_length) {
		if (normal_length < min_length) { normal_length = min_length; }
		if (normal_length > max_length) { normal_length = max_length; }
		return normal_length;
	}
	static const ValueTag Clamp(ValueTag normal_length, ValueTag min_length, ValueTag max_length) {
		if (normal_length.AsUnsigned() < min_length.AsUnsigned()) { normal_length.Set(min_length.AsUnsigned()); }
		if (normal_length.AsUnsigned() > max_length.AsUnsigned()) { normal_length.Set(max_length.AsUnsigned()); }
		return normal_length;
	}
	static const uint CalculatePosition(ValueTag position_low, ValueTag position_high, uint parent_length, uint length) {
		if (position_low.IsAuto() || position_low.IsCenter()) {
			if (position_low.IsCenter()) {
				position_low.Set((((int)parent_length) - (int)length) / 2);
			} else if (!position_high.IsAuto()) {
				position_low.Set((int)parent_length - position_high.AsSigned() - (int)length);
			} else {
				throw style_parse_exception;
			}
		}
		return position_low.AsSigned();
	}
	static const Interval CalculateLength(LengthStyle length, ValueTag position_low, ValueTag position_high, uint parent_length) {
		if (parent_length == 0) { return Interval(0, 0); }

		length = ConvertLengthToPixel(length, parent_length);
		position_low.ConvertToPixel(parent_length);
		position_high.ConvertToPixel(parent_length);

		ValueTag& normal_length = length._normal;
		ValueTag& min_length = length._min;
		ValueTag& max_length = length._max;

		if (normal_length.IsAuto()) {
			if (IsPositionAuto(position_low) || IsPositionAuto(position_high)) {
				normal_length = max_length;
			} else {
				normal_length.Set((int)parent_length - position_low.AsSigned() - position_high.AsSigned());
			}
		}
		normal_length = Clamp(normal_length, min_length, max_length);
		int position = CalculatePosition(position_low, position_high, parent_length, normal_length.AsUnsigned());
		return Interval(position, normal_length.AsUnsigned());
	}
	static auto MakeRectFromInterval(Interval horizontal, Interval vertical) -> const Rect {
		return Rect(horizontal.begin, vertical.begin, horizontal.length, vertical.length);
	}
public:
	const Rect CalculateRegionOnParent(Size parent_size) const {
		return MakeRectFromInterval(
			CalculateLength(width, position._left, position._right, parent_size.width),
			CalculateLength(height, position._top, position._bottom, parent_size.height)
		);
	}
	void ResetRegionOnParent(Rect region_on_parent, Size parent_size) {
		Margin margin_to_parent = CalculateRelativeMargin(parent_size, region_on_parent);
		position.set(px(margin_to_parent.left), px(margin_to_parent.top), px(margin_to_parent.right), px(margin_to_parent.bottom));
		width.normal(px(region_on_parent.size.width)); height.normal(px(region_on_parent.size.height));
	}
	void ResetRegionOnParent(Rect old_region_on_parent, Margin margin_to_extend, Size size_min, Size size_max) {
		if (margin_to_extend.left) {
			uint new_width = (uint)((int)old_region_on_parent.size.width + margin_to_extend.left);
			new_width = Clamp(new_width, size_min.width, size_max.width);
			int new_left = old_region_on_parent.point.x + (int)old_region_on_parent.size.width - (int)new_width;
			width.normal(px(new_width)); position.left(px(new_left));
		}
		if (margin_to_extend.top) {
			uint new_height = (uint)((int)old_region_on_parent.size.height + margin_to_extend.top);
			new_height = Clamp(new_height, size_min.height, size_max.height);
			int new_top = old_region_on_parent.point.y + (int)old_region_on_parent.size.height - (int)new_height;
			height.normal(px(new_height)); position.top(px(new_top));
		}
		if (margin_to_extend.right) { 
			uint new_width = (uint)((int)old_region_on_parent.size.width + margin_to_extend.right);
			new_width = Clamp(new_width, size_min.width, size_max.width);
			width.normal(px(new_width)); position.left(px(old_region_on_parent.point.x));
		}
		if (margin_to_extend.bottom) { 
			uint new_height = (uint)((int)old_region_on_parent.size.height + margin_to_extend.bottom);
			new_height = Clamp(new_height, size_min.height, size_max.height);
			height.normal(px(new_height)); position.top(px(old_region_on_parent.point.y));
		}
	}
	bool HasScrollbar() const {
		return scrollbar._resource->IsVisible();
	}
	const Margin GetScrollbarMargin() const {
		return scrollbar._resource->GetMargin();
	}
	bool HasBorder() const { 
		return border._width > 0 && border._color != color_transparent; 
	}
	unique_ptr<Figure> GetBorder(Size display_size) const { 
		if (border._radius > 0) {
			return std::make_unique<RoundedRectangle>(display_size, border._radius, (float)border._width, border._color);
		} else {
			return std::make_unique<Rectangle>(display_size, (float)border._width, border._color);
		}
	}
	bool HitTestBorder(Size display_size, Point point) const {
		Rect display_region = Rect(point_zero, display_size);
		return display_region.Contains(point) && !ShrinkRegionByLength(display_region, border._width).Contains(point);
	}
	bool IsPointInside(Size display_size, Point point) const {
		return PointInRoundedRectangle(display_size, border._radius, point);
	}
	const Margin CalculateBorderMargin() const {
		return { (int)border._width, (int)border._width, (int)border._width, (int)border._width };
	}
	const Rect GetDisplayRegionWithoutBorder(Size display_size) const {
		return ShrinkRegionByMargin(Rect(point_zero, display_size), CalculateBorderMargin());
	}
	const Margin CalculatePaddingMargin(Size display_size) const {
		PaddingStyle padding = this->padding;
		padding._left.ConvertToPixel(display_size.width);  
		padding._top.ConvertToPixel(display_size.height);  
		padding._right.ConvertToPixel(display_size.width); 
		padding._bottom.ConvertToPixel(display_size.height);
		return { padding._left.AsSigned(), padding._top.AsSigned(), padding._right.AsSigned(), padding._bottom.AsSigned() };
	}
	const Rect CalculateClientRegion(Size displayed_client_size) const {
		ClientStyle client = this->client;
		client._left.ConvertToPixel(displayed_client_size.width);   
		client._top.ConvertToPixel(displayed_client_size.height);   
		LengthStyle width = ConvertLengthToPixel(client.width, displayed_client_size.width);
		LengthStyle height = ConvertLengthToPixel(client.height, displayed_client_size.height);
		if (width._normal.IsAuto()) { width._normal = width._max; }
		width._normal = Clamp(width._normal, width._min, width._max);
		if (height._normal.IsAuto()) { height._normal = height._max; }
		height._normal = Clamp(height._normal, height._min, height._max);
		return Rect(client._left.AsSigned(), client._top.AsSigned(), width._normal.AsUnsigned(), height._normal.AsUnsigned());
	}
	const Rect AutoResizeRegionOnParentToDisplaySize(Size parent_size, Rect region_on_parent, Size display_size, Size size_min, Size size_max) const {
		if (IsRegionHorizontalAuto()) { 
			region_on_parent.size.width = Clamp(display_size.width, size_min.width, size_max.width); 
			region_on_parent.point.x = CalculatePosition(position._left, position._right, parent_size.width, region_on_parent.size.width);
		}
		if (IsRegionVerticalAuto()) {
			region_on_parent.size.height = Clamp(display_size.height, size_min.height, size_max.height);
			region_on_parent.point.y = CalculatePosition(position._top, position._bottom, parent_size.height, region_on_parent.size.height);
		}
		return region_on_parent;
	}
	const Rect AutoResizeClientRegionToContent(Size displayed_client_size, Rect client_region, Rect content_region) const {
		if (client._left.IsAuto()) { client_region.point.x = content_region.point.x; }
		if (client._top.IsAuto()) { client_region.point.y = content_region.point.y; }
		if (client.width._normal.IsAuto()) { 
			LengthStyle width = ConvertLengthToPixel(client.width, displayed_client_size.width);
			client_region.size.width = Clamp(content_region.size.width, width._min.AsUnsigned(), width._max.AsUnsigned());
		}
		if (client.height._normal.IsAuto()) { 
			LengthStyle height = ConvertLengthToPixel(client.height, displayed_client_size.height);
			client_region.size.height = Clamp(content_region.size.height, height._min.AsUnsigned(), height._max.AsUnsigned());
		}
		return client_region;
	}
};


inline const StyleCalculator& GetStyleCalculator(const WndStyle& style) {
	static_assert(sizeof(StyleCalculator) == sizeof(WndStyle));
	return static_cast<const StyleCalculator&>(style);
}


END_NAMESPACE(WndDesign);