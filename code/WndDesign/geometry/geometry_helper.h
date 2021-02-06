#pragma once

#include "../../WndDesignCore/geometry/geometry_helper.h"
#include "margin.h"


BEGIN_NAMESPACE(WndDesign)


inline const Margin MarginNeg(Margin margin) {
	return  { -margin.left, -margin.top, -margin.right, -margin.bottom };
}

inline const Margin MarginAdd(Margin a, Margin b) {
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
	return ShrinkSizeByMargin(size, MarginNeg(margin));
}

inline const Rect ShrinkRegionByMargin(Rect region, Margin margin) {
	region.point.x += margin.left;
	region.point.y += margin.top;
	region.size = ShrinkSizeByMargin(region.size, margin);
	return region;
}

inline const Rect ExtendRegionByMargin(Rect region, Margin margin) {
	return ShrinkRegionByMargin(region, MarginNeg(margin));
}


END_NAMESPACE(WndDesign)