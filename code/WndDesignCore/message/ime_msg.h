#pragma once

#include "msg_base.h"
#include "../system/ime.h"


BEGIN_NAMESPACE(WndDesign)


struct ImeCompositionMsg : _MsgPara {
	const ImeComposition& ime_composition;
	ImeCompositionMsg(const ImeComposition& ime_composition) : ime_composition(ime_composition) {}
};


inline const ImeComposition& GetImeCompositionMsg(Para para) {
	return static_cast<ImeCompositionMsg&>(para).ime_composition;
}


END_NAMESPACE(WndDesign)