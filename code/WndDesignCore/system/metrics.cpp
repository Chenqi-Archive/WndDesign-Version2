#include "metrics.h"

#include "win32_helper.h"


BEGIN_NAMESPACE(WndDesign)


const Size GetDesktopSize() {
	static Size size = []() {	
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0); 
		return Size(static_cast<uint>(rect.right - rect.left), static_cast<uint>(rect.bottom - rect.top));
	}();
	return size;
}


END_NAMESPACE(WndDesign)
