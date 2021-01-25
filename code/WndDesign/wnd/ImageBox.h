#pragma once

#include "WndObject.h"


BEGIN_NAMESPACE(WndDesign)


class FinalWnd : public WndObject {
private:
	WndObject::RegisterChild;
	WndObject::UnregisterChild;
};


class ImageBox : public FinalWnd {
public:
	struct Style : FinalWnd::Style {
		
	};
private:


};


END_NAMESPACE(WndDesign)