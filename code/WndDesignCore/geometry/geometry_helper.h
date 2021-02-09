#pragma once

#include "geometry.h"

#include <cmath>


BEGIN_NAMESPACE(WndDesign)


inline const Point BoundPointInRegion(const Point& point, const Rect& rect) {
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

inline uint square(int x) {
	return static_cast<uint>(x * x); 
};

inline uint SquareDistance(const Point& a, const Point& b) {
	return square(a.x - b.x) + square(a.y - b.y);
}

inline float Distance(const Point& a, const Point& b) {
	return static_cast<float>(sqrt(SquareDistance(a, b)));
}

inline float Distance(const Point& point, const Rect& rect) {
	return Distance(point, BoundPointInRegion(point, rect));
}


// Used when a rectangle is contained in a larger rectangle to calculate the 
//   region of the inner rect's left-top point.
// That is, the returned region is at least 1x1.
inline const Rect ShrinkRegionBySize(const Rect& region, const Size& size) {
	Size new_size = Size(
		(region.size.width > size.width ? region.size.width - size.width : 0) + 1,
		(region.size.height > size.height ? region.size.height - size.height : 0) + 1
	);
	return Rect(region.point, new_size);
}

inline const Rect BoundRectInRegion(Rect rect, const Rect& region) {
	// If rect is larger than region, shrink rect.
	if (rect.size.width > region.size.width) { rect.size.width = region.size.width; }
	if (rect.size.height > region.size.height) { rect.size.height = region.size.height; }
	// Calculate the bounding region for the left top point.
	rect.point = BoundPointInRegion(rect.point, ShrinkRegionBySize(region, rect.size));
	return rect;
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


inline const Point ScalePointBySize(Point point, Size size) {
	return Point(point.x * static_cast<int>(size.width), point.y * static_cast<int>(size.height));
}
inline const Size ScaleSizeBySize(Size old_size, Size size) {
	return Size(old_size.width * size.width, old_size.height * size.height);
}
inline const Rect ScaleRectBySize(Rect region, Size size) {
	return Rect(ScalePointBySize(region.point, size), ScaleSizeBySize(region.size, size));
}


// Divide a by b, and round the result down.
inline int div_floor(int a, int b) {
	if (a >= 0) { return a / b; } else { return (a - b + 1) / b; }
}
// Divide a by b, and round the result up.
inline int div_ceil(int a, int b) {
	if (a <= 0) { return a / b; } else { return (a + b - 1) / b; }
}

inline const Rect RegionToOverlappingTileRange(Rect region, Size tile_size) {
	Point begin_point(div_floor(region.point.x, tile_size.width), div_floor(region.point.y, tile_size.height));
	Point end_point(div_ceil(region.right(), tile_size.width), div_ceil(region.bottom(), tile_size.height));
	Size size = Size(static_cast<uint>(end_point.x - begin_point.x), static_cast<uint>(end_point.y - begin_point.y));
	return Rect(begin_point, size);
}


END_NAMESPACE(WndDesign)