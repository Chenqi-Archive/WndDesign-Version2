#pragma once

#include "geometry.h"

#include <cmath>


BEGIN_NAMESPACE(WndDesign)


inline const Point BoundPointInRegion(Point point, Rect rect) {
	auto BoundNumberInInterval = [](int number, int begin, uint length) -> int {
		if (number < begin) { return begin; }
		if (int end = begin + length; number >= end) { return end - 1; }
		return number;
	};
	return Point(
		BoundNumberInInterval(point.x, rect.point.x, rect.size.width),
		BoundNumberInInterval(point.y, rect.point.y, rect.size.height)
	);
}

inline float Distance(const Point& a, const Point& b) {
	auto square = [](int x) -> uint {return static_cast<uint>(x * x); };
	return static_cast<float>(sqrt(square(a.x - b.x) + square(a.y - b.y)));
}

inline float Distance(const Point& point, const Rect& rect) {
	return Distance(point, BoundPointInRegion(point, rect));
}


// Remind that the returned region includes the right bottom edge.
// That is, the returned region is at least 1x1.
inline const Rect ShrinkRegionBySize(const Rect& region, Size size) {
	return Rect(
		region.point,
		Size(
		(region.size.width > size.width ? region.size.width - size.width : 0) + 1,
		(region.size.height > size.height ? region.size.height - size.height : 0) + 1
	));
}

// Shrink a rect region by length.
inline const Rect ShrinkRegionByLength(const Rect& rect, uint length) {
	uint max_length = min(rect.size.width, rect.size.height) / 2;
	if (length > max_length) { length = max_length; }
	Rect region = rect;
	region.point.x += static_cast<int>(length);
	region.point.y += static_cast<int>(length);
	region.size.width -= 2 * length;
	region.size.height -= 2 * length;
	return region;
}

inline const Rect BoundRectInRegion(Rect rect, const Rect& region) {
	// If rect is larger than region, shrink rect.
	if (rect.size.width > region.size.width) { rect.size.width = region.size.width; }
	if (rect.size.height > region.size.height) { rect.size.height = region.size.height; }
	// Calculate the bounding region for the left top point.
	rect.point = BoundPointInRegion(rect.point, ShrinkRegionBySize(region, rect.size));
	return rect;
}


END_NAMESPACE(WndDesign)