#pragma once

#include "directx_resource.h"


BEGIN_NAMESPACE(WndDesign)


inline IDCompositionDevice& GetDCompDevice() { return *DirectXResources::Get().dcomp_device; }


END_NAMESPACE(WndDesign)