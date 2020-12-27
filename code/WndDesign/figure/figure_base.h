#pragma once

#include "../geometry/geometry.h"


BEGIN_NAMESPACE(WndDesign)

class RenderTarget;  // An alias for ID2D1DeviceContext.


// The Figure abstract base class.
struct Figure {
	// Get bounding region of the figure.
	virtual const Rect GetRegion() const pure;

	// Perform drawing commands to the target (attached to device context).
	// The meaning of offset differs. For example, for rectangle it means the left-top point, 
	//   but for circle it means the center point.
	virtual void DrawOn(RenderTarget& target, Vector offset) const pure;

	// virtual ~Figure() pure {}  // Doesn't need yet.
};


const Size GetTargetSize(const RenderTarget& target);


END_NAMESPACE(WndDesign)