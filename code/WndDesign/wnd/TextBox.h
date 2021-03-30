#pragma once

#include "Wnd.h"
#include "../figure/text_block.h"


BEGIN_NAMESPACE(WndDesign)


class TextBox : public Wnd {
public:
	struct Style : Wnd::Style, TextBlockStyle {
		Style() {
			client.height.min(0px).normal(length_auto).max(length_max_tag);
			client.width.min(0px).normal(length_auto).max(100pct);
		}
	};
	
public:
	TextBox(unique_ptr<Style> style, const wstring& text) :
		Wnd(std::move(style)), _text(text), _text_block(_text, GetStyle()) {
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
	const wstring& GetText() const { return _text; }
	const TextBlock& GetTextBlock() const { return _text_block; }
private:
	void TextLayoutChanged() { 
		Invalidate(region_infinite); 
		ContentLayoutChanged(); 
	}
protected:
	virtual void OnTextChange() { TextLayoutChanged(); }


	// std::wstring wrapper functions
public:
	void SetText(wstring&& text) {
		uint old_length = (uint)_text.length(), new_length = (uint)text.length();
		_text.assign(std::move(text));
		_text_block.TextReplacedWithoutStyle(0, old_length, new_length);
		OnTextChange();
	}
	void SetText(const wstring& text) {
		uint old_length = (uint)_text.length(), new_length = (uint)text.length();
		_text.assign(text);
		_text_block.TextReplacedWithoutStyle(0, old_length, new_length);
		OnTextChange();
	}
	void InsertText(uint pos, wchar ch) {
		_text.insert(pos, 1, ch);
		_text_block.TextInsertedWithoutStyle(pos, 1);
		OnTextChange();
	}
	void InsertText(uint pos, const wstring& str) {
		_text.insert(pos, str);
		_text_block.TextInsertedWithoutStyle(pos, (uint)str.length());
		OnTextChange();
	}
	void ReplaceText(uint begin, uint length, wchar ch) {
		_text.replace(begin, length, 1, ch);
		_text_block.TextReplacedWithoutStyle(begin, length, 1);  // length may be out of range, but it doesn't matter
		OnTextChange();
	}
	void ReplaceText(uint begin, uint length, const wstring& str) {
		_text.replace(begin, length, str);
		_text_block.TextReplacedWithoutStyle(begin, length, (uint)str.length());
		OnTextChange();
	}
	void DeleteText(uint begin, uint length) {
		_text.erase(begin, length);
		_text_block.TextDeleted(begin, length);
		OnTextChange();
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