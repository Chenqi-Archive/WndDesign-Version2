#include "text_style.h"

#include "../system/directx/directx_helper.h"
#include "../system/directx/dwrite_api.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


inline const DWRITE_TEXT_RANGE TextRange2TextRange(TextRange range) {
	return DWRITE_TEXT_RANGE{ range.begin, range.length };
}


enum class TextRangeRelation {
	Left,
	LeftIntersect,
	Overlapped,
	Overlapping,
	RightIntersect,
	Right
};

// a is #relation# relative to b.
inline TextRangeRelation TestTextRangeRelation(TextRange a, TextRange b) {
	assert(!a.IsEmpty() && !b.IsEmpty());
	if (a.right() <= b.left()) { return TextRangeRelation::Left; }
	if (a.right() < b.right() && a.left() < b.left()) { return TextRangeRelation::LeftIntersect; }
	if (a.right() <= b.right() && a.left() >= b.left()) { return TextRangeRelation::Overlapped; }
	if (a.right() >= b.right() && a.left() <= b.left()) { return TextRangeRelation::Overlapping; }
	if (a.right() > b.right() && a.left() > b.left()) { return TextRangeRelation::RightIntersect; }
	if (a.left() >= b.right()) { return TextRangeRelation::Right; }
	assert(0); return TextRangeRelation::Right;
}

list<TextStyleRangeList::TextStyleRange>::iterator TextStyleRangeList::ClearStyle(TextRange range) {
	auto it = styles.begin();
	while (it != styles.end()) {
		switch (TestTextRangeRelation(it->range, range)) {
		case TextRangeRelation::Left:
			it++; break;
		case TextRangeRelation::LeftIntersect:
			it->range.length = range.begin - it->range.begin; it++; break;
		case TextRangeRelation::Overlapped:
			styles.erase(it++); break;
		case TextRangeRelation::Overlapping: {
				uint length = static_cast<uint>(it->range.right() - range.right());
				auto it_prev = it++;
				it_prev->range.length = range.begin - it_prev->range.begin;
				if (it_prev->range.length == 0) {
					it_prev->range.begin = range.right(); it_prev->range.length = length;
					it = it_prev;
				} else if (length != 0) {
					styles.emplace(it, TextStyleRange{ TextRange{range.right(), length}, it_prev->style->Clone() });
					it--;
				}
			}return it;
		case TextRangeRelation::RightIntersect:
			it->range.length = static_cast<uint>(it->range.right() - range.right());
			it->range.begin = range.right();
			return it;
		case TextRangeRelation::Right:
			return it;
		default: assert(0);
		}
	}
	return it;
}

void TextStyleRangeList::SetStyle(TextRange range, const TextStyleBase& style) {
	auto it = ClearStyle(range);
	auto it_prev = it;
	bool extended = false;
	if (it != styles.begin()) {
		it_prev--;
		if (it_prev->range.right() == range.left() && it_prev->style->Equals(style)) {
			it_prev->range.length += range.length;
			extended = true;
		}
	}
	if (it != styles.end()) {
		if (range.right() == it->range.left() && it->style->Equals(style)) {
			if (extended) {
				it_prev->range.length += it->range.length;
				styles.erase(it++);
			} else {
				it->range.length += range.length;
				it->range.begin = range.begin;
				extended = true;
			}
		}
	}
	if (!extended) {
		styles.emplace(it, TextStyleRange{ range, style.Clone() });
	}
}

void TextStyleRangeList::ExtendStyle(TextRange range) {
	auto it = styles.begin();
	while (it != styles.end()) {
		if (it->range.right() < range.left()) { it++; continue; }
		if (it->range.Contains(range.begin - 1) || (it->range.begin == range.begin && range.begin == 0)) {
			it->range.length += range.length;
			it++; break;
		}
		if (it->range.left() >= range.left()) {
			break;
		}
	}
	while (it != styles.end()) {
		it->range.begin += static_cast<int>(range.length);
		it++;
	}
}

void TextStyleRangeList::ShrinkStyle(TextRange range) {
	auto it = ClearStyle(range);
	auto it_prev = it;
	for (auto it_right = it; it_right != styles.end(); ++it_right) {
		it_right->range.begin -= static_cast<int>(range.length);
	}
	if (it != styles.begin()) {
		it_prev--;
		if (it_prev->range.right() == it->range.left() && it_prev->style->Equals(*it->style)) {
			it_prev->range.length += it->range.length;
			styles.erase(it);
		}
	}
}

void TextStyleRangeList::ApplyTo(TextLayout& layout) const {
	for (auto& style : styles) { style.style->ApplyTo(layout, style.range); }
}


void TextStyleFont::ApplyTo(TextLayout& layout, TextRange range) const {
	hr = layout.SetFontFamilyName(value.c_str(), TextRange2TextRange(range));
}

void TextStyleSize::ApplyTo(TextLayout& layout, TextRange range) const {
	hr = layout.SetFontSize(value, TextRange2TextRange(range));
}

void TextStyleBold::ApplyTo(TextLayout& layout, TextRange range) const {
	hr = layout.SetFontWeight(value ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL, TextRange2TextRange(range));
}

void TextStyleItalic::ApplyTo(TextLayout& layout, TextRange range) const {
	hr = layout.SetFontStyle(value ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, TextRange2TextRange(range));
}

void TextStyleUnderline::ApplyTo(TextLayout& layout, TextRange range) const {
	hr = layout.SetUnderline(value, TextRange2TextRange(range));
}

void TextStyleColor::ApplyTo(TextLayout& layout, TextRange range) const {
	hr = layout.SetDrawingEffect(&GetSolidColorBrush(value), TextRange2TextRange(range));
}


END_NAMESPACE(WndDesign)