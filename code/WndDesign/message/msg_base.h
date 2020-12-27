#pragma once

#include "../common/common.h"


BEGIN_NAMESPACE(WndDesign)


enum class Msg : uint;   // see definition in message.h


// Empty base class for message parameter.
struct _MsgPara {};

inline static const _MsgPara nullmsg;

// Usage: void Handler(Msg msg, Para para) {...}
using Para = const _MsgPara&;


END_NAMESPACE(WndDesign)