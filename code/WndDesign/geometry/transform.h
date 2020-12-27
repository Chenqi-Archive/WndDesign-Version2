#pragma once

#include "../common/common.h"


BEGIN_NAMESPACE(WndDesign)


struct Transform {
private:
	float _trans[6];

public:
	static constexpr Transform Identical();
};


END_NAMESPACE(WndDesign)