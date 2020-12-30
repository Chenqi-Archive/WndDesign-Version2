#include "metrics.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)


WNDDESIGNCORE_API const Size GetDesktopSize() {
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	return Size(static_cast<uint>(rect.right - rect.left), static_cast<uint>(rect.bottom - rect.top));
}


END_NAMESPACE(WndDesign)
