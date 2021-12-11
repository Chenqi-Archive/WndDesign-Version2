#pragma once

#include "color.h"
#include "../geometry/geometry.h"
#include "../geometry/interval.h"

#include <memory>
#include <string>
#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::wstring;
using std::vector;

class TextLayout;  // An alias for IDWriteTextLayout.


struct TextStyleBase {
	enum class Type : uint {
		Font,
		Size,
		Bold,
		Italic,
		Underline,
		Color,
		_TypeNumber
	};
	static constexpr size_t _TypeNumber() { return static_cast<size_t>(Type::_TypeNumber); }

	virtual ~TextStyleBase() pure {}

	virtual size_t GetTypeID() const pure;
	virtual bool Equals(const TextStyleBase& style) const pure;
	virtual unique_ptr<TextStyleBase> Clone() const pure;

	virtual void ApplyTo(TextLayout& layout, Interval interval) const pure;
};


class TextStyleIntervalTable {
private:
	struct TextStyleWithInterval {
		unique_ptr<TextStyleBase> style;
		Interval interval;
	};
	vector<TextStyleWithInterval> styles;

	// Reset interval when text changes.
	void OnInsert(uint begin, uint length);
	void OnDelete(uint begin, uint length);

public:
	void AddStyle(const TextStyleBase& style, Interval interval) {

	}
};


struct TextStyleFont : TextStyleBase {
	wstring font;
	TextStyleFont(const wstring& font) : font(font) {}

	virtual size_t GetTypeID() const override { return static_cast<size_t>(Type::Font); }
	virtual bool Equals(const TextStyleBase& style) const override {
		assert(GetTypeID() == style.GetTypeID());
		return (*this).font == static_cast<const TextStyleFont&>(style).font;
	}
	virtual void ApplyTo(TextLayout& layout, Interval interval) const override;
};




END_NAMESPACE(WndDesign)


#pragma once

#include "text_style.h"

#include <array>


BEGIN_NAMESPACE(WndDesign)

using std::array;

class TextLayoutResource;  // Wrapper for IDWriteTextLayout.


// Text format for a single paragraph.
class TextBlock {
private:
	unique_ptr<TextLayoutResource> layout;
	const wstring& text;
	array<TextStyleIntervalTable, TextStyleBase::_TypeNumber()> styles;

public:
	void AddStyle(const TextStyleBase& style, Interval interval) {
		styles[style.GetTypeID()].AddStyle(style, interval);
	}
};



// Helper functions.
inline void TextInsertedResetStyle(TextBlock& text_block, uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles) {
	// Reset layout.

	text_block.ExtendStyle(begin, length);
	text_block.ClearStyle(begin, length);
	for (auto& style : styles) { text_block.SetStyle(begin, length, *style); }
	// Change layout style.
}

inline void TextInsertedMergeStyle(TextBlock& text_block, uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles) {
	text_block.ExtendStyle(begin, length);
	for (auto& style : styles) { text_block.SetStyle(begin, length, *style); }
	// Change layout style.
}

inline void TextInsertedWithoutStyle(TextBlock& text_block, uint begin, uint length) {
	text_block.ExtendStyle(begin, length);
	// Change layout style.
}

inline void TextDeleted(TextBlock& text_block, uint begin, uint length) {
	text_block.ShrinkStyle(begin, length);
}

END_NAMESPACE(WndDesign)

