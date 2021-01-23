#pragma once

#include "wnd_style.h"
#include "../geometry/interval.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


inline const Size CalculateSizeFromSizeTag(WndStyle::SizeStyle::SizeTag size_tag, Size parent_size) {
	size_tag.width.ConvertToPixel(parent_size.width);
	size_tag.height.ConvertToPixel(parent_size.height);
	return Size(size_tag.width.AsUnsigned(), size_tag.height.AsUnsigned());
}

inline std::pair<Size, Size> CalculateMinMaxSizeFromStyle(const WndStyle& style, Size parent_size) {
	return { CalculateSizeFromSizeTag(style.size._min, parent_size), CalculateSizeFromSizeTag(style.size._max, parent_size) };
}


const Interval CalculateLengthFromStyle(ValueTag normal_length, ValueTag min_length, ValueTag max_length,
											   ValueTag position_low, ValueTag position_high, uint parent_length);

inline const Rect MakeRectFromInterval(Interval horizontal, Interval vertical) {
	return Rect(horizontal.begin, vertical.begin, horizontal.length, vertical.length);
}

inline const Rect CalculateRectFromStyle(const WndStyle& style, Size parent_size) {
	const WndStyle::SizeStyle& size = style.size;
	const WndStyle::PositionStyle& position = style.position;
	return MakeRectFromInterval(
		CalculateLengthFromStyle(size._normal.width, size._min.width, size._max.width, position._left, position._right, parent_size.width),
		CalculateLengthFromStyle(size._normal.height, size._min.height, size._max.height, position._top, position._down, parent_size.height)
	);
}


END_NAMESPACE(WndDesign);