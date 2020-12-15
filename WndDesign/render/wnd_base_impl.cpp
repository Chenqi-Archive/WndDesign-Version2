#include "wnd_base_impl.h"

#include <algorithm>


BEGIN_NAMESPACE(WndDesign)


void WndBase::AddChild(IWndBase& child_wnd) {
	WndBase& child = ConvertWnd(child_wnd);
	if (z_index == 0) {
		BaseLayer().
	} else {

		SingleWndLayer& layer = _top_layers.emplace_back();
		_child_wnds.emplace_back(child, layer);
	}
}


END_NAMESPACE(WndDesign)
