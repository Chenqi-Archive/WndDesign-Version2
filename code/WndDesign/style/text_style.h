#pragma once

#include "../common/uncopyable.h"
#include "../figure/color.h"
#include "../geometry/text_range.h"

#include <memory>
#include <string>
#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::wstring;
using std::list;

struct TextLayout;  // An alias for IDWriteTextLayout.


struct ABSTRACT_BASE TextStyleBase {
	enum class Type : uint {
		Font,
		Size,
		Bold,
		Italic,
		Underline,
		Color,
		_TypeNumber
	};
	static constexpr uint _TypeNumber() { return static_cast<uint>(Type::_TypeNumber); }

	virtual ~TextStyleBase() pure {}
	virtual Type GetType() const pure;
	virtual bool Equals(const TextStyleBase& style) const pure;
	virtual unique_ptr<TextStyleBase> Clone() const pure;
	virtual void ApplyTo(TextLayout& layout, TextRange range) const pure;
};


class TextStyleRangeList : Uncopyable {
private:
	struct TextStyleRange {
		TextRange range;
		unique_ptr<TextStyleBase> style;
	};
	list<TextStyleRange> styles;
public:
	TextStyleRangeList() {}
	~TextStyleRangeList() {}
	list<TextStyleRange>::iterator ClearStyle(TextRange range);
	void SetStyle(TextRange range, const TextStyleBase& style);
	void ExtendStyle(TextRange range);
	void ShrinkStyle(TextRange range);
	void ApplyTo(TextLayout& layout) const;
};


#define _DECLARE_TEXT_STYLE(StyleType, ValueType)									\
	struct TextStyle##StyleType : TextStyleBase{									\
		ValueType value;															\
		TextStyle##StyleType(const ValueType& value) : value(value) {}				\
		virtual TextStyleBase::Type GetType() const override { 						\
			return TextStyleBase::Type::StyleType; 									\
		}																			\
		virtual bool Equals(const TextStyleBase& style) const override {			\
			assert(GetType() == style.GetType());									\
			return value == static_cast<const TextStyle##StyleType&>(style).value;	\
		}																			\
		virtual unique_ptr<TextStyleBase> Clone() const override {					\
			return std::make_unique<TextStyle##StyleType>(*this);					\
		}																			\
		virtual void ApplyTo(TextLayout& layout, TextRange range) const override;	\
	}

_DECLARE_TEXT_STYLE(Font, wstring);
_DECLARE_TEXT_STYLE(Size, float);
_DECLARE_TEXT_STYLE(Bold, bool);
_DECLARE_TEXT_STYLE(Italic, bool);
_DECLARE_TEXT_STYLE(Underline, bool);
_DECLARE_TEXT_STYLE(Color, Color);


END_NAMESPACE(WndDesign)