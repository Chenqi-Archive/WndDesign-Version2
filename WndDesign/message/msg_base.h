#pragma once

#include "../common/common.h"


BEGIN_NAMESPACE(WndDesign)


enum class Msg : uint {
	// Message				// Parameters	// Descriptions(Optional)
	_MSG_BEGIN,


	//// Window Message ////
	_WINDOW_MSG_BEGIN,

	Scroll,					// nullmsg		Sent when the window's size has changed, and the sroll bar need to redraw.
	
	_WINDOW_MSG_END,


	//// Mouse Message ////
	_MOUSE_MSG_BEGIN,

	LeftDown,				// MouseMsg&
	LeftUp,					// MouseMsg&
	RightDown,				// MouseMsg&
	RightUp,				// MouseMsg&
	MouseMove,				// MouseMsg&
	MouseWheelVertical,		// MouseMsg&
	MouseWheelHorizontal,	// MouseMsg&

	_MOUSE_MSG_END,


	//// Supplement Mouse Message ////
	_SUPPLEMENT_MOUSE_MESSAGE_BEGIN,

	MouseEnter,				// MouseMsg&	Sent when mouse enters a window region, used to set cursor.
	MouseLeave,				// nullmsg
	LoseCapture,			// nullmsg

	_SUPPLEMENT_MOUSE_MESSAGE_END,


	//// Keyboard Message ////
	_KEYBOARD_MSG_BEGIN,

	KeyDown,				// KeyMsg&
	KeyUp,					// KeyMsg&

	Char,				    // wchar (utf-16 character code. You will only get visible character key messages by GetCharMsg(). )
							// Visible character key messages are expected to be handled in Msg::Char,
							//   while control key messages be handled in Msg::KeyDown.

	ImeStartComposition,	// nullmsg					  (Remember the caret position when starts composition)
	ImeComposition,			// const ImeComposition* (Replace the composition string every time composition updates) 
	ImeEndComposition,		// const ImeComposition* (Commit the result string and reset caret position)
	
	_KEYBOARD_MSG_END,


	// Supplement keyboard Message
	_SUPPLEMENT_KEYBOARD_MESSAGE_BEGIN,

	LoseFocus,				// nullptr

	_SUPPLEMENT_KEYBOARD_MSG_END,


	//// Timer Message ////
	Timer,					// Ref<Timer*>


	//// User Message ////
	_USER_MSG_BEGIN = 0x80000000,


	_MSG_END = 0xFFFFFFFF
};


constexpr inline const Msg UserMsg(uint number) {
	return static_cast<Msg>(static_cast<uint>(Msg::_USER_MSG_BEGIN) + number);
}


// Empty base class for message parameters.
// Usage: void Handler(Msg msg, Para para) {...}
inline static const struct _MsgPara {} nullmsg; 
using Para = const _MsgPara&;


END_NAMESPACE(WndDesign)