#pragma once

#include "msg_base.h"
#include "../geometry/point.h"

BEGIN_NAMESPACE(WndDesign)


struct MouseMsg : _MsgPara{  // 12 bytes
	Point point;
	union {
		struct {
			bool left : 1;
			bool right : 1;
			bool shift : 1;
			bool ctrl : 1;
			bool middle : 1;
			bool xbutton1 : 1;
			bool xbutton2 : 1;
		};
		unsigned short _key_state;
	};
	short wheel_delta;
};


inline bool IsMouseMsg(Msg msg) {
	return msg > Msg::_MOUSE_MSG_BEGIN && msg < Msg::_MOUSE_MSG_END;
}

inline const MouseMsg& GetMouseMsg(Para para) {
	return static_cast<const MouseMsg&>(para);
}


END_NAMESPACE(WndDesign)