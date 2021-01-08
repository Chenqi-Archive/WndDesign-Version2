#pragma once

#include "../layer/desktop_layer.h"


BEGIN_NAMESPACE(WndDesign)


class Desktop {
private:
	Desktop() {}
	~Desktop() {}
public:
	static Desktop& Get();


};


END_NAMESPACE(WndDesign)