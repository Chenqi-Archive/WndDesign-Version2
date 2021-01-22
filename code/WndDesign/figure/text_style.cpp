#include "text_style.h"
#include "../system/directx/directx_helper.h"
#include "../system/directx/dwrite_api.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


inline const DWRITE_TEXT_RANGE Interval2TextRange(Interval interval) {
	assert(interval.begin >= 0);
	return DWRITE_TEXT_RANGE{ static_cast<uint>(interval.begin), interval.length };
}


void TextStyleFont::ApplyTo(TextLayout& layout, Interval interval) const {
	hr = layout.SetFontFamilyName(font.c_str(), Interval2TextRange(interval));
}

void TextStyleSize::ApplyTo(TextLayout& layout, Interval interval) const {
	hr = layout.SetFontSize(size, Interval2TextRange(interval));
}

void TextStyleBold::ApplyTo(TextLayout& layout, Interval interval) const {
	hr = layout.SetFontWeight(bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL, Interval2TextRange(interval));
}

void TextStyleItalic::ApplyTo(TextLayout& layout, Interval interval) const {
	hr = layout.SetFontStyle(italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, Interval2TextRange(interval));
}

void TextStyleUnderline::ApplyTo(TextLayout& layout, Interval interval) const {
	hr = layout.SetUnderline(underline, Interval2TextRange(interval));
}

void TextStyleColor::ApplyTo(TextLayout& layout, Interval interval) const {
	hr = layout.SetDrawingEffect(&GetSolidColorBrush(color), Interval2TextRange(interval));
}


END_NAMESPACE(WndDesign)