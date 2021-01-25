#pragma once

#include "wnd/WndObject.h"
#include "message/message.h"
#include "figure/figure_types.h"
#include "figure/figure_queue.h"
#include "style/style_helper.h"

#include <vector>
#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::vector;
using std::wstring;


extern DesktopObject& desktop;

vector<wstring> GetCommandLineArgs();


END_NAMESPACE(WndDesign)