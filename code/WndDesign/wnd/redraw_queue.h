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
	vector<list<WndBase&>> _queue;
	int _next_depth;

private:
	RedrawQueue();

public:
	void AddWnd(WndBase& wnd);
	void RemoveWnd(WndBase& wnd);
	void Commit();

	static RedrawQueue& Get();
};


END_NAMESPACE(WndDesign)