#include "border_resizer.h"
#include "../wnd/Wnd.h"


BEGIN_NAMESPACE(WndDesign)


void BorderResizer::Handler(Wnd& wnd, Msg msg, Para para) { if (msg == Msg::MouseEnter) { SetCursor(Cursor::Default); } }

const Size BorderResizer::GetWndSize(Wnd& wnd) { return wnd.GetDisplaySize(); }
void BorderResizer::ResizeWnd(Wnd& wnd) {}
void BorderResizer::SetWndCapture(Wnd& wnd) { wnd.SetBorderCapture(); }
void BorderResizer::ReleaseWndCapture(Wnd& wnd) { wnd.ReleaseCapture(); }


class DefaultBorderResizer : public BorderResizer {

};

unique_ptr<BorderResizer> CreateDefaultBorderResizer() {
    return unique_ptr<DefaultBorderResizer>();
}


END_NAMESPACE(WndDesign)