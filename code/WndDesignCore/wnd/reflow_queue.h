#pragma once

#include "../common/core.h"

#include <vector>
#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::vector;
using std::list;

class WndBase;


class ReflowQueue {
private:
	vector<list<ref_ptr<WndBase>>> _queue;
	uint _next_depth;

private:
	ReflowQueue();

public:
	void AddWnd(WndBase& wnd);
	void RemoveWnd(WndBase& wnd);
	void Commit();

	static ReflowQueue& Get();
};

inline ReflowQueue& GetReflowQueue() { return ReflowQueue::Get(); }


END_NAMESPACE(WndDesign)