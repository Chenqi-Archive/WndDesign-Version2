#include "redraw_queue.h"
#include "wnd_base.h"
#include "desktop.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


RedrawQueue::RedrawQueue() : _queue(max_wnd_depth + 1), _next_depth(0) {}

void RedrawQueue::AddWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth(); 
	assert(0 < depth && depth <= max_wnd_depth);
	wnd._redraw_queue_index = _queue[depth].insert(_queue[depth].begin(), &wnd);
	if (depth > _next_depth) { _next_depth = depth; }
}

void RedrawQueue::RemoveWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth();
	assert(0 < depth && depth <= max_wnd_depth);
	_queue[depth].erase(wnd._redraw_queue_index);
	wnd._redraw_queue_index = {};
}

void RedrawQueue::AddDesktopWnd(DesktopWndFrame& frame) {
	frame._redraw_queue_index = _queue[0].insert(_queue[0].begin(), reinterpret_cast<ref_ptr<WndBase>>(&frame));
}

void RedrawQueue::RemoveDesktopWnd(DesktopWndFrame& frame) {
	_queue[0].erase(frame._redraw_queue_index);
	frame._redraw_queue_index = {};
}

void RedrawQueue::Commit() {
	if (_next_depth == 0) { return; }

	BeginDraw();
	uint next_depth = _next_depth;
	_next_depth = 0;
	while (next_depth > 0) {
		while (!_queue[next_depth].empty()) {
			WndBase& wnd = *_queue[next_depth].front();
			wnd.UpdateInvalidRegion();
			wnd.LeaveRedrawQueue();
		}
		next_depth--;
	}
	EndDraw();

	// Present desktop windows whose depth is 0. (Now next_depth must be 0.)
	while (!_queue[next_depth].empty()) {
		DesktopWndFrame& frame = *reinterpret_cast<DesktopWndFrame*>(_queue[next_depth].front());
		frame.Present();
		RemoveDesktopWnd(frame);
	}
}

RedrawQueue& RedrawQueue::Get() {
	static RedrawQueue redraw_queue;
	return redraw_queue;
}


END_NAMESPACE(WndDesign)