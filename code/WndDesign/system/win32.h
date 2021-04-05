#pragma once

#include "../../WndDesignCore/system/win32.h"


BEGIN_NAMESPACE(WndDesign)


void ShowWnd(HANDLE hwnd);

void HideWnd(HANDLE hwnd);

void AddToolWindow(WndObject& wnd);


END_NAMESPACE(WndDesign)