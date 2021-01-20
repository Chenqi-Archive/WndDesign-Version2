#pragma once

#include "../common/core.h"

#include <vector>
#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::vector;
using std::list;

class WndBase;


class RedrawQueue {
private:
	vector<list<ref_ptr<WndBase>>> _queue;
	uint _next_depth;

private:
	RedrawQueue();

public:
	void AddWnd(WndBase& wnd);
	void RemoveWnd(WndBase& wnd);
	void AddDesktopWnd();
	void RemoveDesktopWnd();
	void Commit();

	static RedrawQueue& Get();
};

RedrawQueue& GetRedrawQueue() { return RedrawQueue::Get(); }


END_NAMESPACE(WndDesign)