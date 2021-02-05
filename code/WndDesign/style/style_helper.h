#pragma once

#include "wnd_style.h"
#include "../geometry/interval.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


// a helper class for calculating region from style
struct StyleCalculator : public WndStyle {

	// style check and normalization
private:
	void NormalizeMinMaxLength(LengthStyle& length) {
		if (length._max.IsAuto()) { length._max = length_max_tag; }
		if (length._min.IsAuto()) { length._min = length_min_tag; }
	}
public:
	void Normalize() {
		NormalizeMinMaxLength(width);
		NormalizeMinMaxLength(height);


	}


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
	static bool IsPaddingRelative(const PaddingStyle& padding) {
		return padding._left.IsPercent() || padding._top.IsPercent() || padding._right.IsPercent() || padding._bottom.IsPercent();
	}
	static bool IsScrollbarAuto(const ScrollbarStyle& scrollbar) {
		return scrollbar._left != 0 || scrollbar._top != 0 || scrollbar._right != 0 || scrollbar._bottom != 0;
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
		return IsLengthAuto(width) || IsLengthAuto(height);
	}
	bool IsNonClientMarginRelative() const {
		return IsPaddingRelative(padding);
	}
	bool IsNonClientMarginAuto() const {
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
	static const Interval CalculateLengthForOneDimension(LengthStyle length, ValueTag position_low, ValueTag position_high, uint parent_length) {

	}
public:
	const std::pair<Size, Size> CalculateMinMaxDisplaySize(Size parent_size) const {
		auto CalculateSizeFromSizeTag = [](WndStyle::SizeStyle::SizeTag size_tag, Size parent_size) -> const Size {
			size_tag.width.ConvertToPixel(parent_size.width);
			size_tag.height.ConvertToPixel(parent_size.height);
			return Size(size_tag.width.AsUnsigned(), size_tag.height.AsUnsigned());
		};
		return { CalculateSizeFromSizeTag(style.size._min, parent_size), CalculateSizeFromSizeTag(style.size._max, parent_size) };
	}
	const Rect CalculateRegionOnParent(Size parent_size) const {
		auto MakeRectFromInterval(Interval horizontal, Interval vertical) -> const Rect {
			return Rect(horizontal.begin, vertical.begin, horizontal.length, vertical.length);
		}

		const WndStyle::SizeStyle& size = style.size;
		const WndStyle::PositionStyle& position = style.position;
		return MakeRectFromInterval(
			CalculateLengthFromStyle(size._normal.width, size._min.width, size._max.width, position._left, position._right, parent_size.width),
			CalculateLengthFromStyle(size._normal.height, size._min.height, size._max.height, position._top, position._down, parent_size.height)
		);
	}
	const Margin CalculateNonClientMarginWithoutPadding(Size display_size, Size client_size) const {

	}
	const Margin CalculateNonClientMargin(Size display_size, Margin non_client_margin_without_padding) const {
		PaddingStyle padding = this->padding;
		padding._left.ConvertToPixel(display_size.width); non_client_margin_without_padding.left += padding._left.AsSigned();
		padding._top.ConvertToPixel(display_size.height); non_client_margin_without_padding.top += padding._top.AsSigned();
		padding._right.ConvertToPixel(display_size.width); non_client_margin_without_padding.right += padding._right.AsSigned();
		padding._bottom.ConvertToPixel(display_size.height); non_client_margin_without_padding.bottom += padding._bottom.AsSigned();
		return non_client_margin_without_padding;
	}
	const Rect CalculateClientRegion(Size display_size, Margin non_client_margin) const {

	}
	const Size CalculateDisplaySize(Size client_size, Margin non_client_margin) const {

	}
	const Rect CalculateAccessibleRegion(Rect client_region, Margin non_client_margin) const {

	}
};


inline const StyleCalculator& GetStyleCalculator(WndStyle& style) {
	static_assert(sizeof(StyleCalculator) == sizeof(WndStyle));
	StyleCalculator& style_calculator = static_cast<StyleCalculator&>(style);
	style_calculator.Normalize();
	return style_calculator;
}


END_NAMESPACE(WndDesign);