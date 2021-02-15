#pragma once

#include "../../WndDesignCore/geometry/geometry_helper.h"
#include "margin.h"


BEGIN_NAMESPACE(WndDesign)


inline const Margin operator-(Margin margin) {
	return  { -margin.left, -margin.top, -margin.right, -margin.bottom };
}

inline const Margin operator+(Margin a, Margin b) {
	return {a.left + b.left, a.top + b.top, a.right + b.right, a.bottom + b.bottom };
}

inline const Size ShrinkSizeByMargin(Size size, Margin margin) {
	int width = (int)size.width - (margin.left + margin.right);
	int height = (int)size.height - (margin.top + margin.bottom);
	size.width = width >= 0 ? width : 0;
	size.height = height >= 0 ? height : 0;
	return size;
}

inline const Size ExtendSizeByMargin(Size size, Margin margin) {
	return ShrinkSizeByMargin(size, -margin);
}

inline const Rect ShrinkRegionByMargin(Rect region, Margin margin) {
	region.point.x += margin.left;
	region.point.y += margin.top;
	region.size = ShrinkSizeByMargin(region.size, margin);
	return region;
}

inline const Rect ExtendRegionByMargin(Rect region, Margin margin) {
	return ShrinkRegionByMargin(region, -margin);
}

inline const Margin CalculateRelativeMargin(Size size, Rect region) {
	// ExtentRegionByMargin(region, margin) == Rect(point_zero, size);
	// margin = CalcualteRelativeMargin(size, region);
	return {
		region.point.x, region.point.y,
		(int)size.width - region.point.x - (int)region.size.width, (int)size.height - region.point.y - (int)region.size.height
	};
}


inline bool PointInRoundedRectangle(Size size, uint radius, Point point) {
	if ((uint)point.x >= size.width || (uint)point.y >= size.height) { return false; }
	if (uint max_radius = min(size.width, size.height) / 2; radius > max_radius) { radius = max_radius; }
	int x1 = (int)(radius), x2 = (int)(size.width - radius);
	int	y1 = (int)(radius), y2 = (int)(size.height - radius);
	int x = point.x, y = point.y;
	if (x < x1 && y < y1) { return square(x - x1) + square(y - y1) <= square(radius) ? true : false; }
	if (x < x1 && y > y2) { return square(x - x1) + square(y - y2) <= square(radius) ? true : false; }
	if (x > x2 && y < y1) { return square(x - x2) + square(y - y1) <= square(radius) ? true : false; }
	if (x > x2 && y > y2) { return square(x - x2) + square(y - y2) <= square(radius) ? true : false; }
	return true;
}


END_NAMESPACE(WndDesign)