#pragma once

#include "WndObject.h"


BEGIN_NAMESPACE(WndDesign)


class FinalWnd : public WndObject {
private:
	WndObject::RegisterChild;
	WndObject::UnregisterChild;
};

class ImageBox : public FinalWnd {
	void f() {
		RegisterChild();
	}
};


END_NAMESPACE(WndDesign)