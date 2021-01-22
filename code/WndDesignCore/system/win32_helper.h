#pragma once

#include "../geometry/geometry.h"

#include <Windows.h>
#include <windowsx.h>


BEGIN_NAMESPACE(WndDesign)


inline const RECT Rect2RECT(const Rect& rect) {
    return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

inline const Rect RECT2Rect(const RECT& rect) {
    return Rect(rect.left, rect.top, static_cast<uint>(rect.right - rect.left), static_cast<uint>(rect.bottom - rect.top));
}


END_NAMESPACE(WndDesign)