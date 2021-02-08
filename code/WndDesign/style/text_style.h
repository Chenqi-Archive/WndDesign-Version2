#pragma once

#include "../figure/color.h"
#include "../common/uncopyable.h"

#include <memory>
#include <string>
#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::wstring;
using std::list;

class TextLayout;  // An alias for IDWriteTextLayout.


struct TextRange {
	uint begin;
	uint length;

	uint left() const { return begin; }
	uint right() const { return begin + length; }

	bool IsEmpty() const { return length == 0; }
	bool Contains(uint pos) const { return pos >= left() && pos < right(); }
};


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


template <TextStyleBase::Type type, class ValueType>
struct _TextStyleType : TextStyleBase {
	ValueType value;
	_TextStyleType(const ValueType& value) : value(value) {}
	virtual TextStyleBase::Type GetType() const override { return type; }
	virtual bool Equals(const TextStyleBase& style) const override {
		assert(GetType() == style.GetType());
		return this->value == static_cast<const _TextStyleType&>(style).value;
	}
	virtual unique_ptr<TextStyleBase> Clone() const override {
		return std::make_unique<_TextStyleType>(*this);
	}
};

struct TextStyleFont : _TextStyleType<TextStyleBase::Type::Font, wstring> {
	virtual void ApplyTo(TextLayout& layout, TextRange range) const override;
};

struct TextStyleSize : _TextStyleType<TextStyleBase::Type::Size, float> {
	virtual void ApplyTo(TextLayout& layout, TextRange range) const override;
};

struct TextStyleBold : _TextStyleType<TextStyleBase::Type::Bold, bool> {
	virtual void ApplyTo(TextLayout& layout, TextRange range) const override;
};

struct TextStyleItalic : _TextStyleType<TextStyleBase::Type::Italic, bool> {
	virtual void ApplyTo(TextLayout& layout, TextRange range) const override;
};

struct TextStyleUnderline : _TextStyleType<TextStyleBase::Type::Underline, bool> {
	virtual void ApplyTo(TextLayout& layout, TextRange range) const override;
};

struct TextStyleColor : _TextStyleType<TextStyleBase::Type::Color, Color> {
	virtual void ApplyTo(TextLayout& layout, TextRange range) const override;
};


END_NAMESPACE(WndDesign)