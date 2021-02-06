#pragma once

#include "wnd_style.h"
#include "../geometry/interval.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


// a helper class for calculating region from style
struct StyleCalculator : public WndStyle {
private:
	StyleCalculator() = delete;

private:
	static inline const std::invalid_argument style_parse_exception = std::invalid_argument("style parse error");

	// style dependency identification
private:
	static bool IsLengthRelative(const LengthStyle& length) {
		return length._normal.IsPercent() || length._min.IsPercent() || length._max.IsPercent();
	}
	static bool IsLengthAuto(const LengthStyle& length) {
		return length._normal.IsAuto();
	}
	static bool IsPositionRelative(const PositionStyle& position) {
		return position._left.IsPercent() || position._top.IsPercent() || position._right.IsPercent() || position._bottom.IsPercent();
	}
	static bool IsPositionAuto(ValueTag position_low, ValueTag position_high) {
		return position_low.IsAuto() || position_high.IsAuto() || position_low.IsCenter() || position_high.IsCenter();
	}
	static bool IsPaddingRelative(const PaddingStyle& padding) {
		return padding._left.IsPercent() || padding._top.IsPercent() || padding._right.IsPercent() || padding._bottom.IsPercent();
	}
	static bool IsScrollbarAuto(const ScrollbarStyle& scrollbar) {
		return scrollbar._scrollbar_resource->HasMargin();
	}
	static bool IsClientRelative(const ClientStyle& client) {
		return client._left.IsPercent() || client._top.IsPercent() || client._width.IsPercent() || client._height.IsPercent();
	}
	static bool IsClientAuto(const ClientStyle& client) {
		return client._width.IsAuto() || client._height.IsAuto();
	}
public:
	bool IsRegionOnParentRelative() const {
		return IsLengthRelative(width) || IsLengthRelative(height) || IsPositionRelative(position);
	}
	bool IsRegionOnParentAuto() const {
		return (IsLengthAuto(width) && IsPositionAuto(position._left, position._right)) ||
			(IsLengthAuto(height) && IsPositionAuto(position._top, position._bottom));
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
private:
	static const Size CalculateSizeFromTag(ValueTag width, ValueTag height, Size parent_size) {
		width.ConvertToPixel(parent_size.width); height.ConvertToPixel(parent_size.height);
		return Size(width.AsUnsigned(), height.AsUnsigned());
	}
public:
	const std::pair<Size, Size> CalculateMinMaxDisplaySize(Size parent_size) const {
		return { CalculateSizeFromTag(width._min, height._min, parent_size), CalculateSizeFromTag(width._max, height._max, parent_size) };
	}
private:
	static const LengthStyle ConvertLengthToPixel(LengthStyle length, uint parent_length) {
		length._normal.ConvertToPixel(parent_length);
		length._min.ConvertToPixel(parent_length);
		length._max.ConvertToPixel(parent_length);
		return length;
	}
	static const ValueTag BoundLengthBetween(ValueTag& normal_length, ValueTag min_length, ValueTag max_length) {
		if (normal_length.AsUnsigned() < min_length.AsUnsigned()) { normal_length.Set(min_length.AsUnsigned()); }
		if (normal_length.AsUnsigned() > max_length.AsUnsigned()) { normal_length.Set(max_length.AsUnsigned()); }
		return normal_length;
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
			if (IsPositionAuto(position_low, position_high)) {
				normal_length = max_length;
			} else {
				normal_length.Set(position_high.AsSigned() - position_low.AsSigned());
			}
		}
		BoundLengthBetween(normal_length, min_length, max_length);
		if (position_low.IsAuto() || position_low.IsCenter()) {
			if (position_low.IsCenter()) {
				position_low.Set((static_cast<int>(parent_length) - normal_length.AsSigned()) / 2);
			} else if (!position_high.IsAuto()) {
				position_low.Set(static_cast<int>(parent_length) - position_high.AsSigned() - normal_length.AsSigned());
			} else {
				throw style_parse_exception;
			}
		}
		return Interval(position_low.AsSigned(), normal_length.AsUnsigned());
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
	const Margin GetScrollBarMargin() const {
		return scrollbar._scrollbar_resource->GetMargin();
	}
	bool UpdateScrollBar(Rect displayed_client_region_with_padding, Rect client_region_with_padding) const {
		return scrollbar._scrollbar_resource->Update(displayed_client_region_with_padding, client_region_with_padding);
	}
	const Margin CalculateMarginWithoutPadding(Margin margin_scrollbar) const {
		return {
			border._width + margin_scrollbar.left, border._width + margin_scrollbar.top,
			border._width + margin_scrollbar.right, border._width + margin_scrollbar.bottom,
		};
	}
	const Margin CalculateMargin(Size display_size, Margin margin_without_padding) const {
		PaddingStyle padding = this->padding;
		padding._left.ConvertToPixel(display_size.width); margin_without_padding.left += padding._left.AsSigned();
		padding._top.ConvertToPixel(display_size.height); margin_without_padding.top += padding._top.AsSigned();
		padding._right.ConvertToPixel(display_size.width); margin_without_padding.right += padding._right.AsSigned();
		padding._bottom.ConvertToPixel(display_size.height); margin_without_padding.bottom += padding._bottom.AsSigned();
		return margin_without_padding;
	}
	const Rect CalculateClientRegion(Size displayed_client_size) const {
		ClientStyle client = this->client;
		client._left.ConvertToPixel(displayed_client_size.width);   
		client._top.ConvertToPixel(displayed_client_size.height);   
		client._width.ConvertToPixel(displayed_client_size.width);  
		client._height.ConvertToPixel(displayed_client_size.height);
		return Rect(client._left.AsSigned(), client._top.AsSigned(), client._width.AsUnsigned(), client._height.AsUnsigned());
	}
	const Size AutoResizeRegionOnParentToDisplaySize(Size region_on_parent_size, Size display_size) const {
		if (IsLengthAuto(width) && IsPositionAuto(position._left, position._right)) { region_on_parent_size.width = display_size.width; }
		if (IsLengthAuto(height) && IsPositionAuto(position._top, position._bottom)) { region_on_parent_size.height = display_size.height; }
		return region_on_parent_size;
	}
	const Rect AutoResizeClientRegionToContent(Rect client_region, Rect content_region) const {
		if (client._left.IsAuto()) { client_region.point.x = content_region.point.x; }
		if (client._top.IsAuto()) { client_region.point.y = content_region.point.y; }
		if (client._width.IsAuto()) { client_region.size.width = content_region.size.width; }
		if (client._height.IsAuto()) { client_region.size.height = content_region.size.height; }
		return client_region;
	}
};


inline const StyleCalculator& GetStyleCalculator(WndStyle& style) {
	static_assert(sizeof(StyleCalculator) == sizeof(WndStyle));
	return static_cast<StyleCalculator&>(style);
}


END_NAMESPACE(WndDesign);