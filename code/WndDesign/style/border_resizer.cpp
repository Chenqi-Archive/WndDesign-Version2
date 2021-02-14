#include "border_resizer.h"


BEGIN_NAMESPACE(WndDesign)


class DefaultBorderResizer : public BorderResizer {

};

unique_ptr<BorderResizer> CreateDefaultBorderResizer() {
    return unique_ptr<BorderResizer>();
}


END_NAMESPACE(WndDesign)