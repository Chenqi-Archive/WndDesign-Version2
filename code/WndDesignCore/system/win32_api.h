#pragma once

#include "../geometry/geometry.h"


BEGIN_NAMESPACE(WndDesign)

using HANDLE = void*;


BEGIN_NAMESPACE(Win32)


HANDLE CreateWnd(Rect region, const wstring& title);
void DestroyWnd(HANDLE hWnd);

void SetWndUserData(HANDLE hWnd, void* data);

void MoveWnd(HANDLE hWnd, Rect region);
void SetWndTitle(HANDLE hWnd, const wstring& title);

void SetCapture(HANDLE hWnd);
void ReleaseCapture();

void SetFocus(HANDLE hWnd);
void ReleaseFocus();

int MessageLoop();


END_NAMESPACE(Win32)

END_NAMESPACE(WndDesign)