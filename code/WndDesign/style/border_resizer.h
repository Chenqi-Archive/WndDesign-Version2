#pragma once

#include "../message/message.h"
#include "../geometry/geometry.h"
#include "../geometry/margin.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class Wnd;


class BorderResizer {
public:
	BorderResizer() {}
	virtual ~BorderResizer() {}
public:
	virtual void Handler(Wnd& wnd, Rect window_region, uint border_width, Msg msg, Para para);
protected:
	void ResizeWnd(Wnd& wnd, Rect old_window_region, Margin margin_to_extend);
	void SetWndCapture(Wnd& wnd);
	void ReleaseWndCapture(Wnd& wnd);
};


unique_ptr<BorderResizer> CreateEmptyBorderResizer();
unique_ptr<BorderResizer> CreateDefaultBorderResizer();
unique_ptr<BorderResizer> CreateAeroSnapBorderResizer();


END_NAMESPACE(WndDesign)