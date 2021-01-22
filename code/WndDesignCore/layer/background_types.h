#pragma once

#include "background_base.h"


BEGIN_NAMESPACE(WndDesign)


struct NullBackground : public Background {
private:
	NullBackground();
public:
	static const NullBackground& Get();
	virtual void Clear(Rect region, RenderTarget& target, Vector offset) const override;  // defined in figure_types.cpp
	virtual void DrawOn(Rect region, RenderTarget& target, Vector offset, uchar opacity) const override;
};


END_NAMESPACE(WndDesign)