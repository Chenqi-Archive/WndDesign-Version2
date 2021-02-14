#pragma once

#include "../geometry/geometry.h"
#include "../geometry/margin.h"
#include "../message/message.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class FigureQueue;
class Wnd;


class Scrollbar {
public:
	Scrollbar() {}
	virtual ~Scrollbar() {}

	virtual bool IsAuto() const { return false; }
	virtual bool HasMargin() const { return false; }
	virtual const Margin GetMargin() const { return {}; }
	virtual void Update(Size entire_size, Rect display_region) {}

	virtual bool IsVisible() const { return false; }
	virtual void OnPaint(FigureQueue& figure_queue, Size display_size) {}

	virtual bool HitTest(Size display_size, Point point) { return false; }
	virtual bool Handler(Wnd& wnd, Msg msg, Para para) { return false; }

protected:
	void SetWndCapture(Wnd& wnd) { wnd.SetScrollbarCapture(); }
	void ReleaseWndCapture(Wnd& wnd) { wnd.ReleaseCapture(); }
};


unique_ptr<Scrollbar> CreateEmptyScrollbar() { return std::make_unique<Scrollbar>(); }

unique_ptr<Scrollbar> CreateDefaultScrollbar();


END_NAMESPACE(WndDesign)