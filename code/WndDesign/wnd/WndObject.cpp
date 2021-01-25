#include "WndObject.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


inline const Rect WndObject::CalculateRegionOnParent(Size parent_size) {
	return CalculateRectFromStyle(*this, parent_size);
}

inline void WndObject::OnSizeChange(Rect accessible_region) {
	// change size or position style according to accessible region.
	// also when region on parent changed.
}

inline const pair<Size, Size> WndObject::CalculateMinMaxSize(Size parent_size) {
	return CalculateMinMaxSizeFromStyle(*this, parent_size);
}

inline const wstring WndObject::GetTitle() const {
	return L"MainWnd";
}


END_NAMESPACE(WndDesign)