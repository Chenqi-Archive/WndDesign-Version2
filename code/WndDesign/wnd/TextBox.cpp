#include "TextBox.h"
#include "../figure/figure_queue.h"


BEGIN_NAMESPACE(WndDesign)


const Rect TextBox::UpdateContentLayout(Size client_size) {
	_text_block.AutoResize(client_size);
	return Rect(point_zero, _text_block.GetSize());
}

void TextBox::OnClientPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
	figure_queue.Append(point_zero, new TextBlockFigure(_text_block));
}


END_NAMESPACE(WndDesign)