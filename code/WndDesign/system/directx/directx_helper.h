#pragma once

#include "../../../WndDesignCore/system/directx/directx_helper.h"

#include <wincodec.h>
#include <dwrite_3.h>


BEGIN_NAMESPACE(WndDesign)


struct TextLayout : IDWriteTextLayout3 {};  // alias for IDWriteTextLayout3


END_NAMESPACE(WndDesign)