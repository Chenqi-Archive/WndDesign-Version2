#pragma once

#include "rect.h"


BEGIN_NAMESPACE(WndDesign)


class RectPointIterator {
private:
	Point _start, _end;
	Point _current_point;

public:
	RectPointIterator(Rect rect) : _start(rect.point), _end(rect.RightBottom()), _current_point(_start) {}
	bool Finished() const { return _current_point.x >= _end.x || _current_point.y >= _end.y; }
	const Point Item() const { 
		assert(!Finished());
		return _current_point;
	}
	void operator++() {
		assert(!Finished());
		_current_point.x++;
		if (_current_point.x >= _end.x) { _current_point.x = _start.x; _current_point.y++; }
	}
};


END_NAMESPACE(WndDesign)