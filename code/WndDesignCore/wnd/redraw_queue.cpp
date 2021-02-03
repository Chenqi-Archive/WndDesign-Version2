#include "redraw_queue.h"
#include "wnd_base.h"
#include "desktop.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)

inline static const uint max_wnd_depth = WndBase::max_wnd_depth;  // depth <= 63

static bool has_begun_commit = false;  // window relation should not change when painting


RedrawQueue::RedrawQueue() : _queue(max_wnd_depth + 1), _next_depth(0) {}

void RedrawQueue::AddWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth(); 
	assert(0 < depth && depth <= max_wnd_depth);
	assert(!wnd._redraw_queue_index.valid());
	wnd._redraw_queue_index = _queue[depth].insert(_queue[depth].begin(), &wnd);
	if (depth > _next_depth) { _next_depth = depth; }
}

void RedrawQueue::RemoveWnd(WndBase& wnd) {
	if (has_begun_commit) { throw std::logic_error("window removed when committing redraw queue"); }
	uint depth = wnd.GetDepth();
	assert(0 < depth && depth <= max_wnd_depth);
	assert(wnd._redraw_queue_index.valid());
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

	has_begun_commit = true;
	BeginDraw();
	uint next_depth = _next_depth;
	_next_depth = 0;
	while (next_depth > 0) {
		while (!_queue[next_depth].empty()) {
			WndBase& wnd = *_queue[next_depth].front();
			wnd.UpdateInvalidRegion();
			RemoveWnd(wnd);
		}
		next_depth--;
	}
	EndDraw();
	has_begun_commit = false;

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