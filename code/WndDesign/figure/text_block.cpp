#include "text_block.h"
#include "../system/directx/directx_helper.h"
#include "../system/directx/dwrite_api.h"


BEGIN_NAMESPACE(WndDesign)


IDWriteTextFormat* AsTextFormat(TextLayout* text_format) { return reinterpret_cast<IDWriteTextFormat*>(text_format); }
IDWriteTextLayout* AsTextLayout(TextLayout* text_layout) { return reinterpret_cast<IDWriteTextLayout*>(text_layout); }
IDWriteTextFormat** AsTextFormat(TextLayout** text_format) { return reinterpret_cast<IDWriteTextFormat**>(text_format); }
IDWriteTextLayout** AsTextLayout(TextLayout** text_layout) { return reinterpret_cast<IDWriteTextLayout**>(text_layout); }


TextBlock::TextBlock(wstring& text, TextBlockStyle& style) :
	_text(text), _style(style), _format(nullptr), _layout(nullptr), _max_size(size_max), _size() {
	TextChanged();
}

TextBlock::~TextBlock() {
	SafeRelease(AsTextLayout(&_layout));
	SafeRelease(AsTextFormat(&_format));
}

void TextBlock::UpdateSize() const {
	DWRITE_TEXT_METRICS metrics;
	_layout->GetMetrics(&metrics);
	_size = Size(
		static_cast<uint>(ceil(metrics.widthIncludingTrailingWhitespace)),  // Round up the size.
		static_cast<uint>(ceil(metrics.height))
	);
}

void TextBlock::TextChanged() {
	// Recreate TextFormat and TextLayout.
	SafeRelease(AsTextLayout(&_layout));
	SafeRelease(AsTextFormat(&_format));
	hr = GetDWriteFactory().CreateTextFormat(
		_style.font._family.c_str(),
		NULL,
		static_cast<DWRITE_FONT_WEIGHT>(_style.font._weight),
		static_cast<DWRITE_FONT_STYLE>(_style.font._style),
		static_cast<DWRITE_FONT_STRETCH>(_style.font._stretch),
		static_cast<FLOAT>(_style.font._size),
		_style.font._locale.c_str(),
		AsTextFormat(&_format)
	);
	hr = GetDWriteFactory().CreateTextLayout(
		_text.c_str(), static_cast<UINT>(_text.length()),
		AsTextFormat(_format),
		(FLOAT)_max_size.width, (FLOAT)_max_size.height,
		AsTextLayout(&_layout)
	);

	// Set paragraph styles.
	_layout->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(_style.paragraph._text_align));
	_layout->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(_style.paragraph._paragraph_align));
	_layout->SetFlowDirection(static_cast<DWRITE_FLOW_DIRECTION>(_style.paragraph._flow_direction));
	_layout->SetReadingDirection(static_cast<DWRITE_READING_DIRECTION>(_style.paragraph._read_direction));
	_layout->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(_style.paragraph._word_wrap));
	ValueTag line_height = _style.paragraph._line_height;
	if (line_height.IsPixel()) {
		_layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, static_cast<FLOAT>(line_height.AsUnsigned()), 0.7F * line_height.AsUnsigned());
	} else if (line_height.IsPercent()) {
		_layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, line_height.AsUnsigned() / 100.0F, 1.1F);
	} else {
		_layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 0.0F, 0.0F);  // The last two parameters are ignored.
	}
	if (_style.paragraph._tab_size != 0) { _layout->SetIncrementalTabStop(static_cast<FLOAT>(_style.paragraph._tab_size)); }

	// Set text range styles and update layout size.
	ApplyAllStyles();
	UpdateSize();
}

void TextBlock::AutoResize(Size max_size) const {
	if (_max_size == max_size) { return; }

	if (_max_size.width != max_size.width) {
		_max_size.width = max_size.width; _layout->SetMaxWidth(static_cast<FLOAT>(_max_size.width));
	}
	if (_max_size.height != max_size.height) {
		_max_size.height = max_size.height; _layout->SetMaxHeight(static_cast<FLOAT>(_max_size.height));
	}

	UpdateSize();
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

		// Add width for empty lines.
		if (geometry_regions[i].geometry_region.size.width < 5) { geometry_regions[i].geometry_region.size.width = 5; }
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
	ClearStyle(begin, length, true);
	TextChanged(); // Just recreate the layout.
}

void TextBlock::TextDeleted(uint begin, uint length) {
	ShrinkStyle(begin, length);
	TextChanged();
}

void TextBlock::TextInsertedResetStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles) {
	ExtendStyle(begin, length);
	ClearStyle(begin, length, true);
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


END_NAMESPACE(WndDesign)