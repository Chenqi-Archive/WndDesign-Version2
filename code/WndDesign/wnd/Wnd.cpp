#include "Wnd.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


const Rect Wnd::CalculateRegionOnParent(Size parent_size) {
	// 1. Calculate min max size based on parent size.
	
	// 2. If size is not auto, the region is calculated.

	// If size is auto

}

void Wnd::OnSizeChange(Rect accessible_region) {
	// change size or position style according to accessible region.
	// also when region on parent changed.
}

const pair<Size, Size> Wnd::CalculateMinMaxSize(Size parent_size) {
	return CalculateMinMaxSizeFromStyle(*_style, parent_size);
}

const wstring& Wnd::GetTitle() const {
	return L"MainWnd";
}

const Wnd::NonClientRegion Wnd::CalculateNonClientRegion() {
	WndStyle& style = GetStyle();
	NonClientRegion non_client_region;
	non_client_region.left = style.border._width + style.padding._left;
	non_client_region.top = style.border._width;
	non_client_region.right = style.border._width;
	non_client_region.bottom = style.border._width;

	return non_client_region;
}


END_NAMESPACE(WndDesign)