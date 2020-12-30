//#include "wnd_base_impl.h"
//
//#include <algorithm>
//
//
//BEGIN_NAMESPACE(WndDesign)
//
//
//bool ShouldAllocateTopLayer(CompositeStyle& style) {
//
//}
//
//
//void WndBase::AddChild(IWndBase& child_wnd) {
//	WndBase& child = ConvertWnd(child_wnd);
//	if (IsMyChild(child)) { return; }
//
//	CompositeStyle composite_style = child.GetCompositeStyle();
//	if (z_index == 0) {
//		BaseLayer().
//	} else {
//		SingleWndLayer& layer = _top_layers.emplace_back();
//		_child_wnds.emplace_back(child, layer);
//	}
//}
//
//void WndBase::RemoveChild(IWndBase& child_wnd) {
//}
//
//void WndBase::UpdateChildPosition(WndBase& child_wnd) {
//}
//
//void WndBase::UpdateChildCompositeStyle(WndBase& child_wnd) {
//}
//
//
//END_NAMESPACE(WndDesign)
