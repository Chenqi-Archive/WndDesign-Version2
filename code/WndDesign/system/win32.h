#pragma once

#include "../../WndDesignCore/system/win32.h"


BEGIN_NAMESPACE(WndDesign)


void ShowWnd(HANDLE hwnd);

void HideWnd(HANDLE hwnd);

void HideWndFromTaskbar(HANDLE hwnd);


END_NAMESPACE(WndDesign)