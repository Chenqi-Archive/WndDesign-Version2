#pragma once

#include "../message/message.h"


#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;


class BorderResizer {




};


unique_ptr<BorderResizer> CreateEmptyBorderResizer() { return std::make_unique<BorderResizer>(); }

unique_ptr<BorderResizer> CreateDefaultBorderResizer();


END_NAMESPACE(WndDesign)