#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


struct Margin {
	int left; 
	int top; 
	int right; 
	int bottom;

	bool IsEmpty() const { return left == 0 && top == 0 && right == 0 && bottom == 0; }
};


END_NAMESPACE(WndDesign)