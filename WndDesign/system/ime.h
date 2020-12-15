#pragma once

#include "../common/common.h"
#include "../figure/color.h"
#include "../geometry/rect.h"

#include <string>
#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::wstring;
using std::vector;


// The status of an ongoing ime composition.
struct ImeComposition {
	struct UnderlineStyle {
		uint begin = 0;
		uint end = 0;
		Color color = ColorSet::White;
		bool thick = false;
	};

	uint begin = 0;	// The start position of the selection range.
	uint end = 0;	// The end of the selection range.
	enum class Type : uint { None = 0, CompositionString = 0x0008, ResultString = 0x0800 } type = Type::None;
	wstring string;					   // The string retrieved.
	vector<UnderlineStyle> underlines; // The underline information of the composition string.
};


END_NAMESPACE(WndDesign)