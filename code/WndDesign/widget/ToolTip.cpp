#include "ToolTip.h"


BEGIN_NAMESPACE(WndDesign)


ToolTip& ToolTip::Get() {
	static ToolTip tool_tip(L"");
	return tool_tip;
}


END_NAMESPACE(WndDesign)