#pragma once

#include "../style/text_style.h"
#include "figure_base.h"

#include <array>
#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::array;
using std::vector;

class TextLayoutResource;  // Wrapper for IDWriteTextLayout.


// Text layout for a single paragraph.
class TextLayout {
private:
	unique_ptr<TextLayoutResource> layout;
	const wstring& text; 
	TextStyle default_style;

public:
	TextLayout(const wstring& text, TextStyle default_style = {}) : text(text) {}

	TextLayoutResource& LayoutResource() const { return *layout; }
	const TextStyle& DefaultStyle() const { return default_style; }

	// Size.


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	class TextStyleIntervalList {
		struct TextStyleInterval {
			Interval interval;
			unique_ptr<TextStyleBase> style;
		};
		list<TextStyleInterval> styles;
	public:
		list<TextStyleInterval>::iterator ClearStyle(Interval interval);
		void SetStyle(Interval interval, const TextStyleBase& style);
		void ExtendStyle(Interval interval);
		void ShrinkStyle(Interval interval);
		void ApplyTo(TextLayout& layout) const;
	};
	array<TextStyleIntervalList, TextStyleBase::_TypeNumber()> styles;

	void _SetStyle(uint begin, uint length, const TextStyleBase& style);
	void _ClearStyle(uint begin, uint length);
	void _ExtendStyle(uint begin, uint length);
	void _ShrinkStyle(uint begin, uint length);
	void _ApplyAllStyles();

public:
	// Set or clear the style of a text interval.
	void SetStyle(uint begin, uint length, const TextStyleBase& style);
	void ClearStyle(uint begin, uint length);

public:
	// Update layout and styles when text inserted or deleted.
	void TextInsertedResetStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles);
	void TextInsertedMergeStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles);
	void TextInsertedWithoutStyle(uint begin, uint length);
	void TextDeleted(uint begin, uint length);
};


struct TextLayoutFigure : Figure {
	const TextLayout& text_layout;

	TextLayoutFigure(const TextLayout& text_layout) : text_layout(text_layout) {}
	virtual const Rect GetRegion() const override;
	virtual void DrawOn(RenderTarget& target, Vector offset) const override; // defined in figure_types.cpp
};


END_NAMESPACE(WndDesign)