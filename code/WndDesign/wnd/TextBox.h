#pragma once

#include "FinalWnd.h"
#include "../figure/text_block.h"


BEGIN_NAMESPACE(WndDesign)


class TextBox : public FinalWnd {
public:
	struct Style : FinalWnd::Style, TextBlockStyle {};
	
public:
	TextBox(unique_ptr<Style> style, wstring& text) :FinalWnd(std::move(style)), _text_block(text, *style) {}
	~TextBox() {}


	//// style ////
protected:
	Style& GetStyle() { return static_cast<Style&>(Wnd::GetStyle()); }
	const Style& GetStyle() const { return static_cast<const Style&>(Wnd::GetStyle()); }


	//// layout update ////
private:
	TextBlock _text_block;
public:
	TextBlock& GetTextBlock() { return _text_block; }
	void TextUpdated() { ContentLayoutChanged(); }
private:
	virtual const Rect UpdateContentLayout(Size client_size);


	//// painting and composition ////
protected:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const override;


	//// message handling ////
public:

};


END_NAMESPACE(WndDesign)