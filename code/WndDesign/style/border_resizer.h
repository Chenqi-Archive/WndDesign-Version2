#pragma once

#include "../message/message.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class Wnd;


class BorderResizer {
public:
	BorderResizer() {}
	virtual ~BorderResizer() {}
	virtual bool HitTest(Size display_size, uint border_width);  // radius ?
	virtual void Handler(Wnd& wnd, Msg msg, Para para) {}

protected:
	const Size GetWndSize(Wnd& wnd);
	void ResizeWnd(Wnd& wnd);
	void SetWndCapture(Wnd& wnd) { wnd.SetBorderCapture(); }
	void ReleaseWndCapture(Wnd& wnd) { wnd.ReleaseCapture(); }
};


unique_ptr<BorderResizer> CreateEmptyBorderResizer() { return std::make_unique<BorderResizer>(); }

unique_ptr<BorderResizer> CreateDefaultBorderResizer();


END_NAMESPACE(WndDesign)