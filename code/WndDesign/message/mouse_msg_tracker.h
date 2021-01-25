#pragma once

#include "message.h"
#include "../wnd/WndObject.h"

#include <ctime>


BEGIN_NAMESPACE(WndDesign)


// Mouse track widget. Used to track mouse LEFT button.
enum class MouseTrackMsg {
	None,
	Click,             // down - (move < 5px) - up
	DoubleClick,       // down - up - down(<0.5s)
	TripleClick,       // down - up - down(<0.5s) - up - down(<0.5s)
	Drag,              // down - move
};


class MouseTracker {
public:
	Point mouse_current_position;
	bool has_mouse_down;
	uint mouse_down_time;
	Point mouse_down_position;

public:
	MouseTrackMsg Track(Msg msg, Para para) {
		if (!IsMouseMsg(msg)) { return MouseTrackMsg::None; }
		switch (msg) {
		case Msg::LeftDown:
			break;
		case Msg::LeftUp:
			break;
		case Msg::MouseMove:
			break;
		default:
			break;
		}
	}
};


END_NAMESPACE(WndDesign)