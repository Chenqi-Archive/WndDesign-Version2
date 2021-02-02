#pragma once

#include "../../WndDesignCore/geometry/geometry_helper.h"
#include "margin.h"


BEGIN_NAMESPACE(WndDesign)


inline const Rect ShrinkRegionByMargin(Rect region, Margin margin) {
	region.point.x += margin.left;
	region.point.y += margin.top;
	int width = (int)region.size.width - (margin.left + margin.right);
	int height = (int)region.size.height - (margin.top + margin.bottom);
	region.size.width = width >= 0 ? width : 0;
	region.size.height = height >= 0 ? height : 0;
	return region;
}


END_NAMESPACE(WndDesign)