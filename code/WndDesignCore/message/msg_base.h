#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


enum class Msg : uint;   // see definition in message.h


// Empty base class for message parameter.
struct _MsgPara {};

inline static _MsgPara nullmsg;

// Usage: void Handler(Msg msg, Para para) {...}
using Para = _MsgPara&;


END_NAMESPACE(WndDesign)