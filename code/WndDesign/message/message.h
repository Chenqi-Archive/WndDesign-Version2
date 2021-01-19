#pragma once

#include "msg_base.h"
#include "mouse_msg.h"
#include "key_msg.h"
#include "ime_msg.h"


BEGIN_NAMESPACE(WndDesign)


enum class Msg : uint {
	// message				// parameters	// descriptions (optional)
	_MSG_BEGIN,


	//// mouse message ////
	_MOUSE_MSG_BEGIN,

	LeftDown,				// MouseMsg&
	LeftUp,					// MouseMsg&
	RightDown,				// MouseMsg&
	RightUp,				// MouseMsg&
	MouseMove,				// MouseMsg&
	MouseWheelVertical,		// MouseMsg&
	MouseWheelHorizontal,	// MouseMsg&

	_MOUSE_MSG_END,


	//// supplement mouse message ////
	_SUPPLEMENT_MOUSE_MESSAGE_BEGIN,

	MouseEnter,				// nullmsg     Sent when mouse enters a window region, used to set cursor.
	MouseLeave,				// nullmsg
	LoseCapture,			// nullmsg

	_SUPPLEMENT_MOUSE_MESSAGE_END,


	//// keyboard message ////
	_KEYBOARD_MSG_BEGIN,

	KeyDown,				// KeyMsg&
	KeyUp,					// KeyMsg&

	Char,				    // wchar (utf-16 character code. You will only get visible character key messages by GetCharMsg(). )
							// Visible character key messages are expected to be handled in Msg::Char,
							//   while control key messages be handled in Msg::KeyDown.

	ImeCompositionBegin,	// nullmsg				 (Remember the caret position when starts composition)
	ImeComposition,			// const ImeComposition* (Replace the composition string every time composition updates) 
	ImeCompositionEnd,		// const ImeComposition* (Commit the result string and reset caret position)
	
	_KEYBOARD_MSG_END,


	//// supplement keyboard message ////
	_SUPPLEMENT_KEYBOARD_MESSAGE_BEGIN,

	LoseFocus,				// nullptr

	_SUPPLEMENT_KEYBOARD_MSG_END,


	_MSG_END = 0xFFFFFFFF
};


inline bool IsKeyboardMsg(Msg msg) {
	return msg > Msg::_KEYBOARD_MSG_BEGIN && msg < Msg::_KEYBOARD_MSG_END;
}

inline bool IsMouseMsg(Msg msg) {
	return msg > Msg::_MOUSE_MSG_BEGIN && msg < Msg::_MOUSE_MSG_END;
}


END_NAMESPACE(WndDesign)