#pragma once

#include "../layer/composite_effect.h"


BEGIN_NAMESPACE(WndDesign)


struct CompositeStyle : CompositeEffect {
	uint z_index = 0;
};


END_NAMESPACE(WndDesign)