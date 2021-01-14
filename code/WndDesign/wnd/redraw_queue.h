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
	vector<list<WndBase&>> queue;
	int next_depth;

public:
	RedrawQueue();
	void AddWnd(WndBase& wnd);
	void RemoveWnd(WndBase& wnd);
	void Commit();
};


END_NAMESPACE(WndDesign)