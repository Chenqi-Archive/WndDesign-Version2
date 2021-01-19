#include "redraw_queue.h"
#include "wnd_base.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


inline static const uint max_wnd_depth = 63;


RedrawQueue::RedrawQueue() : _queue(max_wnd_depth + 1), _next_depth(0) {}

void RedrawQueue::AddWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth(); 
	assert(depth > 0);	if (depth > max_wnd_depth) { throw std::invalid_argument("window hierarchy too deep"); }
	assert(!wnd.HasRedrawQueueIndex());
	wnd.SetRedrawQueueIndex(_queue[depth].insert(_queue[depth].begin(), &wnd));
	if (depth > _next_depth) { _next_depth = depth; }
}

void RedrawQueue::RemoveWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth();
	assert(depth <= max_wnd_depth);
	assert(wnd.HasRedrawQueueIndex());
	_queue[depth].erase(wnd.GetRedrawQueueIndex());
	wnd.SetRedrawQueueIndex();
}

void RedrawQueue::AddDesktopWnd() {
}

void RedrawQueue::RemoveDesktopWnd() {
}

void RedrawQueue::Commit() {
	BeginDraw();

	uint next_depth = _next_depth;
	while (next_depth > 0) {
		while (!_queue[next_depth].empty()) {
			WndBase& wnd = *_queue[next_depth].front();
			wnd.UpdateInvalidRegion();
			RemoveWnd(wnd);
		}
		next_depth--;
	}

	EndDraw();

	// Present system windows.

}

RedrawQueue& RedrawQueue::Get() {
	static RedrawQueue redraw_queue;
	return redraw_queue;
}


END_NAMESPACE(WndDesign)