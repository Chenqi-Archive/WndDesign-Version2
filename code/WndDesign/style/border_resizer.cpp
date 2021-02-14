#include "border_resizer.h"
#include "../wnd/Wnd.h"


BEGIN_NAMESPACE(WndDesign)


class DefaultBorderResizer : public BorderResizer {

};

unique_ptr<BorderResizer> CreateDefaultBorderResizer() {
    return unique_ptr<BorderResizer>();
}


END_NAMESPACE(WndDesign)