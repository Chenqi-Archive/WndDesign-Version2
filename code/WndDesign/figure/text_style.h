#pragma once

#include "color.h"
#include "../geometry/interval.h"

#include <memory>
#include <string>
#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::wstring;
using std::list;

using std::make_unique;


struct TextStyle {
	const wchar* font = L"Arial";
	float size = 16.0f;
	bool bold = false;
	bool italic = false;
	bool underline = false;
	Color color = ColorSet::Black;
};


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


struct TextStyleFont : TextStyleBase {
	wstring font;
	TextStyleFont(const wstring& font) : font(font) {}

	virtual size_t GetTypeID() const override { return static_cast<size_t>(Type::Font); }
	virtual bool Equals(const TextStyleBase& style) const override {
		assert(GetTypeID() == style.GetTypeID());
		return (*this).font == static_cast<const TextStyleFont&>(style).font;
	}
	virtual unique_ptr<TextStyleBase> Clone() const override {
		return make_unique<TextStyleFont>(*this);
	}
	virtual void ApplyTo(TextLayout& layout, Interval interval) const override;
};

struct TextStyleSize : TextStyleBase {
	float size;
	TextStyleSize(float size) : size(size) {}

	virtual size_t GetTypeID() const override { return static_cast<size_t>(Type::Size); }
	virtual bool Equals(const TextStyleBase& style) const override {
		assert(GetTypeID() == style.GetTypeID());
		return (*this).size == static_cast<const TextStyleSize&>(style).size;
	}
	virtual unique_ptr<TextStyleBase> Clone() const override {
		return make_unique<TextStyleSize>(*this);
	}
	virtual void ApplyTo(TextLayout& layout, Interval interval) const override;
};


#define _DECLARE_TEXT_STYLE(style_type, value_type, value)									\
	struct TextStyle##style_type : TextStyleBase{											\
		value_type value;																	\
		TextStyle##style_type(const value_type& value) : value(value){}						\
		virtual size_t GetTypeID() const override { 										\
			return static_cast<size_t>(Type::style_type); 									\
		}																					\
		virtual bool Equals(const TextStyleBase& style) const override {					\
			assert(GetTypeID() == style.GetTypeID());										\
			return (*this).value == static_cast<const TextStyle##style_type&>(style).value; \
		}																					\
		virtual unique_ptr<TextStyleBase> Clone() const override {							\
			return make_unique<TextStyle##style_type>(*this);								\
		}																					\
		virtual void ApplyTo(TextLayout& layout, Interval interval) const override;			\
	}


_DECLARE_TEXT_STYLE(Bold, bool, bold);
_DECLARE_TEXT_STYLE(Italic, bool, italic);
_DECLARE_TEXT_STYLE(Underline, bool, underline);
_DECLARE_TEXT_STYLE(Color, Color, color);


END_NAMESPACE(WndDesign)