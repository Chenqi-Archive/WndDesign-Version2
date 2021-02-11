#include "text_block.h"
#include "../system/directx/directx_helper.h"
#include "../system/directx/dwrite_api.h"


BEGIN_NAMESPACE(WndDesign)


IDWriteTextFormat* AsTextFormat(TextLayout* text_format) { return reinterpret_cast<IDWriteTextFormat*>(text_format); }
IDWriteTextLayout* AsTextLayout(TextLayout* text_layout) { return reinterpret_cast<IDWriteTextLayout*>(text_layout); }
IDWriteTextFormat** AsTextFormat(TextLayout** text_format) { return reinterpret_cast<IDWriteTextFormat**>(text_format); }
IDWriteTextLayout** AsTextLayout(TextLayout** text_layout) { return reinterpret_cast<IDWriteTextLayout**>(text_layout); }


TextBlock::TextBlock(wstring& text, TextBlockStyle& style) :
	_text(text), _style(style), _format(nullptr), _layout(nullptr), _max_size(), _size() {
	hr = GetDWriteFactory().CreateTextFormat(
		style.font._family.c_str(),
		NULL,
		static_cast<DWRITE_FONT_WEIGHT>(style.font._weight),
		static_cast<DWRITE_FONT_STYLE>(style.font._style),
		static_cast<DWRITE_FONT_STRETCH>(style.font._stretch),
		static_cast<FLOAT>(style.font._size),
		style.font._locale.c_str(),
		AsTextFormat(&_format)
	);
	TextChanged();
}

TextBlock::~TextBlock() {
	SafeRelease(AsTextFormat(&_format));
	SafeRelease(AsTextLayout(&_layout));
}

void TextBlock::TextChanged() {
	SafeRelease(AsTextLayout(&_layout));
	hr = GetDWriteFactory().CreateTextLayout(
		_text.c_str(), static_cast<UINT>(_text.length()),
		AsTextFormat(_format),
		(FLOAT)_max_size.width, (FLOAT)_max_size.height,
		AsTextLayout(&_layout)
	);
	AutoResize(_max_size);
	ApplyAllStyles();
}

void TextBlock::AutoResize(Size max_size) {
	if (_max_size == max_size) { return; }

	if (_max_size.width != max_size.width) {
		_max_size.width = max_size.width; _layout->SetMaxWidth(static_cast<FLOAT>(_max_size.width));
	}
	if (_max_size.height != max_size.height) {
		_max_size.height = max_size.height; _layout->SetMaxHeight(static_cast<FLOAT>(_max_size.height));
	}

	DWRITE_TEXT_METRICS metrics;
	_layout->GetMetrics(&metrics);

	_size = Size(
		static_cast<uint>(ceil(metrics.widthIncludingTrailingWhitespace)),  // Round up the size.
		static_cast<uint>(ceil(metrics.height))
	);
}

const TextBlockHitTestInfo TextBlock::HitTestPoint(Point point) const {
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS metrics;
	_layout->HitTestPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y), &isTrailingHit, &isInside, &metrics);
	return TextBlockHitTestInfo{
		metrics.textPosition,
		metrics.length,
		static_cast<bool>(isInside),
		static_cast<bool>(isTrailingHit),
		Rect(POINT2Point(D2D1::Point2F(metrics.left, metrics.top)), SIZE2Size(D2D1::SizeF(metrics.width, metrics.height))),
	};
}

const TextBlockHitTestInfo TextBlock::HitTestTextPosition(uint text_position) const {
	FLOAT x, y;
	DWRITE_HIT_TEST_METRICS metrics;
	_layout->HitTestTextPosition(text_position, false, &x, &y, &metrics);
	return TextBlockHitTestInfo{
		metrics.textPosition,
		metrics.length,
		true,
		false,
		Rect(POINT2Point(D2D1::Point2F(metrics.left, metrics.top)), SIZE2Size(D2D1::SizeF(metrics.width, metrics.height))),
	};
}

void TextBlock::HitTestTextRange(uint text_position, uint text_length, vector<TextBlockHitTestInfo>& geometry_regions) const {
	vector<DWRITE_HIT_TEST_METRICS> metrics;

	UINT32 line_cnt;
	_layout->GetLineMetrics(nullptr, 0, &line_cnt);

	UINT32 actual_size = line_cnt; // The assumed actual line size.
	do {
		metrics.resize(actual_size);
		_layout->HitTestTextRange(text_position, text_length, 0, 0, metrics.data(), static_cast<UINT32>(metrics.size()), &actual_size);
	} while (actual_size > metrics.size());
	metrics.resize(actual_size);

	geometry_regions.resize(metrics.size());
	for (size_t i = 0; i < geometry_regions.size(); ++i) {
		geometry_regions[i] = TextBlockHitTestInfo{
			metrics[i].textPosition,
			metrics[i].length,
			true,
			false,
			Rect(POINT2Point(D2D1::Point2F(metrics[i].left, metrics[i].top)), SIZE2Size(D2D1::SizeF(metrics[i].width, metrics[i].height))),
		};
	}
}

void TextBlock::SetStyle(uint begin, uint length, const TextStyleBase& style, bool internal_use_tag) {
	_range_styles[(uint)style.GetType()].SetStyle(TextRange{ begin, length }, style);
}

void TextBlock::ClearStyle(uint begin, uint length, bool internal_use_tag) {
	for (auto& style : _range_styles) { style.ClearStyle(TextRange{ begin, length }); }
}

void TextBlock::ExtendStyle(uint begin, uint length) {
	for (auto& style : _range_styles) { style.ExtendStyle(TextRange{ begin, length }); }
}

void TextBlock::ShrinkStyle(uint begin, uint length) {
	for (auto& style : _range_styles) { style.ShrinkStyle(TextRange{ begin, length }); }
}

void TextBlock::ApplyAllStyles() {
	for (auto& style : _range_styles) { style.ApplyTo(*_layout); }
}

void TextBlock::SetStyle(uint begin, uint length, const TextStyleBase& style) {
	SetStyle(begin, length, style, true);
	style.ApplyTo(*_layout, TextRange{ begin, length });
}

void TextBlock::ClearStyle(uint begin, uint length) {
	ClearStyle(begin, length);
	TextChanged(); // Just recreate the layout.
}

void TextBlock::TextInsertedResetStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles) {
	ExtendStyle(begin, length);
	ClearStyle(begin, length);
	for (auto& style : styles) { SetStyle(begin, length, *style, true); }
	TextChanged();
}

void TextBlock::TextInsertedMergeStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles) {
	ExtendStyle(begin, length);
	for (auto& style : styles) { SetStyle(begin, length, *style, true); }
	TextChanged();
}

void TextBlock::TextInsertedWithoutStyle(uint begin, uint length) {
	ExtendStyle(begin, length);
	TextChanged();
}

void TextBlock::TextDeleted(uint begin, uint length) {
	ShrinkStyle(begin, length);
	TextChanged();
}


END_NAMESPACE(WndDesign)