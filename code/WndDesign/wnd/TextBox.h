#pragma once

#include "FinalWnd.h"
#include "../figure/text_layout.h"
#include "../style/text_style.h"


BEGIN_NAMESPACE(WndDesign)


class TextBox : public FinalWnd {
public:
	using Style = TextBoxStyle;

private:
	TextLayout _layout;
	

public:
	TextBox(const wstring& text) {}

protected:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
		figure_queue.Append(point_zero, new TextLayoutFigure(_layout));
	}

	struct HitTestInfo {

	};
};


END_NAMESPACE(WndDesign)