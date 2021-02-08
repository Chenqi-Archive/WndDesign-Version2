#pragma once

#include "figure_base.h"
#include "../style/text_block_style.h"
#include "../style/text_style.h"

#include <array>
#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::array;
using std::vector;


class TextBlock : Uncopyable {
public:
	TextBlock(wstring& text, TextBlockStyle& style);
	~TextBlock();


	//// text layout ////
private:
	const wstring& _text;
	const TextBlockStyle& _style;
private:
	alloc_ptr<TextLayout> _format;
	alloc_ptr<TextLayout> _layout;
	Size _max_size;
	Size _size;

public:
	const Size GetSize() const { return _size; }
	TextLayout& GetLayout() const { return *_layout; }
	const TextBlockStyle& GetDefaultStyle() const { return _style; }
private:
	void TextChanged();
public:
	void AutoResize(Size max_size);
public:
	const TextBlockHitTestInfo HitTestPoint(Point point) const;
	const TextBlockHitTestInfo HitTestTextPosition(uint text_position) const;
	void HitTestTextRange(uint text_position, uint text_length, vector<TextBlockHitTestInfo>& geometry_regions) const;


	//// text range style ////
private:
	array<TextStyleRangeList, TextStyleBase::_TypeNumber()> _range_styles;

private:
	void SetStyle(uint begin, uint length, const TextStyleBase& style, bool internal_use_tag);
	void ClearStyle(uint begin, uint length, bool internal_use_tag);
	void ExtendStyle(uint begin, uint length);
	void ShrinkStyle(uint begin, uint length);
	void ApplyAllStyles();

public:
	// Set or clear the style of a text range.
	void SetStyle(uint begin, uint length, const TextStyleBase& style);
	void ClearStyle(uint begin, uint length);

public:
	// Update layout and styles when text inserted or deleted.
	void TextInsertedResetStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles);
	void TextInsertedMergeStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles);
	void TextInsertedWithoutStyle(uint begin, uint length);
	void TextDeleted(uint begin, uint length);
};


struct TextBlockFigure : Figure {
	const TextBlock& text_block;

	TextBlockFigure(const TextBlock& text_block) : text_block(text_block) {}
	virtual const Rect GetRegion() const override { return Rect(point_zero, text_block.GetSize()); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override; // defined in figure_types.cpp
};


END_NAMESPACE(WndDesign)