#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


struct CompositeEffect {
	uchar _opacity = 0xFF;
	uchar _blur_radius = 0;
	char _z_index = 0;  // -128 ~ 127 (bottom ~ topmost)
	bool _mouse_penetrate = false;

	bool IsTopmost() const { return _z_index == (char)0x7F; }
	bool IsBottom() const { return _z_index == (char)0x80; }
};


END_NAMESPACE(WndDesign)