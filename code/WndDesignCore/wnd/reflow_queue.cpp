#include "reflow_queue.h"
#include "wnd_base.h"


BEGIN_NAMESPACE(WndDesign)


ReflowQueue::ReflowQueue() : _queue(max_wnd_depth + 1), _next_depth(0) {}

void ReflowQueue::AddWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth();
	assert(0 < depth && depth <= max_wnd_depth);
	wnd._reflow_queue_index = _queue[depth].insert(_queue[depth].begin(), &wnd);
	if (depth > _next_depth) { _next_depth = depth; }
}

void ReflowQueue::RemoveWnd(WndBase& wnd) {
	uint depth = wnd.GetDepth();
	assert(0 < depth && depth <= max_wnd_depth);
	_queue[depth].erase(wnd._reflow_queue_index);
	wnd._reflow_queue_index = {};
}

void ReflowQueue::Commit() {
	if (_next_depth == 0) { return; }

	// Traverse for the first time from back to front, notify parent window if region may change.
	for (uint next_depth = _next_depth; next_depth > 0; next_depth--) {
		for (auto wnd : _queue[next_depth]) {
			wnd->MayRegionOnParentChange();
		}
	}

	// Traverse and update for the second time.
	for (uint next_depth = 1; next_depth <= _next_depth; next_depth++) {
		while (!_queue[next_depth].empty()) {
			WndBase& wnd = *_queue[next_depth].front();
			wnd.UpdateInvalidLayout();
			wnd.LeaveReflowQueue();
		}
	}

	_next_depth = 0;
}

ReflowQueue& ReflowQueue::Get() {
	static ReflowQueue reflow_queue;
	return reflow_queue;
}


END_NAMESPACE(WndDesign)