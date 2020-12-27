#pragma once

#include "../common/common.h"


BEGIN_NAMESPACE(WndDesign)

struct IWndBase;


struct IDesktop {
	WNDDESIGN_API static IDesktop& Get();

	virtual void AddChild(IWndBase& child_wnd) pure;
	virtual void RemoveChild(IWndBase& child_wnd) pure;

	virtual int MessageLoop() pure;
	virtual void Finish() pure;
};


END_NAMESPACE(WndDesign)