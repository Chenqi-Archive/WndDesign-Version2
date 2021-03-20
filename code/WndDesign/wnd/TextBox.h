#pragma once

#include "FinalWnd.h"
#include "../figure/text_block.h"


BEGIN_NAMESPACE(WndDesign)


class TextBox : public FinalWnd {
public:
	struct Style : FinalWnd::Style, TextBlockStyle {};
	
public:
	TextBox(unique_ptr<Style> style, const wstring& text) :
		FinalWnd(std::move(style)), _text(text), _text_block(_text, GetStyle()) {
	}
	~TextBox() {}


	//// style ////
protected:
	Style& GetStyle() { return static_cast<Style&>(Wnd::GetStyle()); }
	const Style& GetStyle() const { return static_cast<const Style&>(Wnd::GetStyle()); }


	//// text layout ////
private:
	wstring _text;
	TextBlock _text_block;
public:
	wstring& GetText() { return _text; }
	TextBlock& GetTextBlock() { return _text_block; }
	void TextBlockUpdated() { 
		Invalidate(region_infinite); 
		ContentLayoutChanged(); 
	}
protected:
	virtual const Rect UpdateContentLayout(Size client_size) {
		Size old_size = _text_block.GetSize();
		_text_block.AutoResize(client_size);
		if (_text_block.GetSize() != old_size) { Invalidate(region_infinite); }
		return Rect(point_zero, _text_block.GetSize());
	}
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const override {
		figure_queue.Append(point_zero, new TextBlockFigure(_text_block));
	}
};


END_NAMESPACE(WndDesign)