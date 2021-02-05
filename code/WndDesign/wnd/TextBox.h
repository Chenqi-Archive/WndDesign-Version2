#pragma once

#include "FinalWnd.h"
#include "../figure/text_layout.h"
#include "../style/text_style.h"


BEGIN_NAMESPACE(WndDesign)


class TextBox : public FinalWnd {
public:
	using Style = FinalWnd::Style;
	

public:
	TextBox(unique_ptr<Style> style, wstring& text) : FinalWnd(std::move(style)), _layout(text) {}
	~TextBox();


private:
	TextLayout _layout;

public:
	void TextUpdated() {
		
		ChildLayoutChanged();
	}


protected:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const override {
		figure_queue.Append(point_zero, new TextLayoutFigure(_layout));
	}

	struct HitTestInfo {

	};


	const HitTestInfo HitTestPoint(Point point) {

	}
};


END_NAMESPACE(WndDesign)