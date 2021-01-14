#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


struct CompositeEffect {
	uchar opacity = 0xFF;
	// shadow effect.
	// blur effect.

	bool HasEffect() const {
		return 
			opacity != 0xFF ||
			// other effects.
			false;
	}
};


END_NAMESPACE(WndDesign)