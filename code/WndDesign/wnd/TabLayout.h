#pragma once

#include "WndObject.h"
#include "ListLayout.h"
#include "TextBox.h"


BEGIN_NAMESPACE(WndDesign)


class TabLayout : WndObject {
public:
	class ChildWndFrame : public TextBox {
	private:
		WndObject& child;
		wstring title;
	public:

	};
	ListLayout _tabs;

public:


};


END_NAMESPACE(WndDesign)