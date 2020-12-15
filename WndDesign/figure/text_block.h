#pragma once

#include "text_style.h"

#include <array>
#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::array;
using std::vector;

class TextLayoutResource;  // Wrapper for IDWriteTextLayout.


// Text format for a single paragraph.
class TextBlock {
private:
	unique_ptr<TextLayoutResource> layout;
	const wstring& text;

public:
	TextBlock(const wstring& text /*, initial styles*/) : text(text) {}

	TextLayoutResource& LayoutResource() const { return *layout; }
	const TextStyle& DefaultStyle() const {}

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


END_NAMESPACE(WndDesign)