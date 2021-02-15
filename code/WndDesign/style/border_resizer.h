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
	virtual void Handler(Wnd& wnd, Msg msg, Para para);

protected:
	const Size GetWndSize(Wnd& wnd);
	void ResizeWnd(Wnd& wnd);
	void SetWndCapture(Wnd& wnd);
	void ReleaseWndCapture(Wnd& wnd);
};


unique_ptr<BorderResizer> CreateEmptyBorderResizer();
unique_ptr<BorderResizer> CreateDefaultBorderResizer();


END_NAMESPACE(WndDesign)