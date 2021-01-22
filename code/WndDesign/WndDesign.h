#pragma once

#include "common/core.h"
#include "wnd/WndObject.h"

#include <vector>
#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::vector;
using std::wstring;


extern DesktopObject& desktop;

vector<wstring> GetCommandLineArgs();


END_NAMESPACE(WndDesign)