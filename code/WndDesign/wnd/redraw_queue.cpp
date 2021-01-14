#include "redraw_queue.h"
#include "wnd_base.h"


BEGIN_NAMESPACE(WndDesign)


inline static const uint max_wnd_depth = 63;


RedrawQueue::RedrawQueue() : queue(max_wnd_depth + 1), next_depth(0) {}

void RedrawQueue::AddWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth(); 
	assert(depth > 0);	if (depth > max_wnd_depth) { throw std::invalid_argument("window hierarchy too deep"); }
	assert(!wnd.HasRedrawQueueIndex());
	wnd.SetRedrawQueueIndex(queue[depth].insert(queue[depth].begin(), wnd));
	if (depth > next_depth) { next_depth = depth; }
}

void RedrawQueue::RemoveWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth();
	assert(depth <= max_wnd_depth);
	assert(wnd.HasRedrawQueueIndex());
	queue[depth].erase(wnd.GetRedrawQueueIndex());
	wnd.SetRedrawQueueIndex();
}

void RedrawQueue::Commit() {
	// BeginDraw.

	while (next_depth > 0) {
		while (!queue[next_depth].empty()) {
			WndBase& wnd = queue[next_depth].front(); queue[next_depth].pop_front();
			wnd.UpdateInvalidRegion();
		}
		next_depth--;
	}

	// EndDraw

	// Present system windows.

}


END_NAMESPACE(WndDesign)