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

	// scroll offset update
public:
	virtual void Update(Size entire_size, Rect display_region) {}

	// margin calculation
public:
	virtual bool IsMarginAuto() const { return false; }
	virtual bool HasMargin() const { return false; }
	virtual const Margin GetMargin() const { return {}; }

	// painting
private:
	virtual void SizeUpdated(Wnd& wnd) {}
public:
	virtual bool IsVisible() const { return false; }
	virtual void OnPaint(FigureQueue& figure_queue) {}

	// hit-testing and message handling
public:
	virtual bool HitTest(Point point) { return false; }
	virtual bool Handler(Wnd& wnd, Msg msg, Para para) { return false; }

	// Wnd functions
private:
	Rect _region;
public:
	void SetRegion(Wnd& wnd, Rect region) { _region = region; SizeUpdated(wnd); }
	const Rect GetRegion() const { return _region; }
protected:
	const Size GetDisplaySize() const { return _region.size; }
	void SetWndCapture(Wnd& wnd);
	void ReleaseWndCapture(Wnd& wnd);
	void InvalidateWnd(Wnd& wnd, Rect region);
	void SendWndClientMessage(Wnd& wnd, Msg msg, Para para);
	void SetWndScrollOffset(Wnd& wnd, Vector offset);
};


unique_ptr<Scrollbar> CreateEmptyScrollbar();
unique_ptr<Scrollbar> CreateDefaultScrollbar();


END_NAMESPACE(WndDesign)