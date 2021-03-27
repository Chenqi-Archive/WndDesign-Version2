#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


struct TextRange {
	uint begin;
	uint length;

	uint left() const { return begin; }
	uint right() const { return begin + length; }

	bool IsEmpty() const { return length == 0; }
	bool Contains(uint pos) const { return pos >= left() && pos < right(); }
};


END_NAMESPACE(WndDesign)