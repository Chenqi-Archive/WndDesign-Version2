#pragma once

#include "wnd/DesktopObject.h"
#include "figure/figure_types.h"

#include <vector>
#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::vector;
using std::wstring;


vector<wstring> GetCommandLineArgs();


END_NAMESPACE(WndDesign)