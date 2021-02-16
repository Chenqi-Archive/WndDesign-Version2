#include "TextBox.h"


BEGIN_NAMESPACE(WndDesign)


TextBox::TextBox(unique_ptr<Style> style, const wstring& text) :
	FinalWnd(std::move(style)), _text(text), _text_block(_text, GetStyle()) {
}

TextBox::~TextBox() {}

const Rect TextBox::UpdateContentLayout(Size client_size) {
	if (_text_block.AutoResize(client_size)) {
		Invalidate(region_infinite);
	}
	return Rect(point_zero, _text_block.GetSize());
}

void TextBox::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {
	figure_queue.Append(point_zero, new TextBlockFigure(_text_block));
}


END_NAMESPACE(WndDesign)