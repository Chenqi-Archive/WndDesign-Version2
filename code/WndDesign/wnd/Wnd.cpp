#include "Wnd.h"
#include "../figure/figure_queue.h"
#include "../geometry/geometry_helper.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


Wnd::Wnd(unique_ptr<Style> style) : _style(std::move(style)) {
	if (style == nullptr) { throw std::invalid_argument("style can't be null"); }
	SetBackground(_style->background.get());
}

Wnd::~Wnd() {}


const pair<Size, Size> Wnd::CalculateMinMaxSize(Size parent_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	return style.CalculateMinMaxDisplaySize(parent_size);
}

const wstring& Wnd::GetTitle() const {
	return GetStyle().title._title;
}

void Wnd::SetRegionStyle(Rect parent_specified_region) {
	Style& style = GetStyle();
	style.position.left(px(parent_specified_region.point.x));
	style.position.top(px(parent_specified_region.point.y));
	style.width.normal(px(parent_specified_region.size.width));
	style.height.normal(px(parent_specified_region.size.height));
}

bool Wnd::MayRegionOnParentChange() {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	if (_invalid_layout.content_layout && style.IsClientRegionAuto()) { _invalid_layout.client_region = true; }
	if (_invalid_layout.client_region && style.IsMarginAuto()) { _invalid_layout.margin = true; }
	if ((_invalid_layout.client_region || _invalid_layout.margin) && style.IsRegionOnParentAuto()) { _invalid_layout.region_on_parent = true; }
	return _invalid_layout.region_on_parent ? true : false;
}

void Wnd::UpdateLayout() {
#ifdef _DEBUG
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
#endif
	if (_invalid_layout.region_on_parent) {
		WndObject::UpdateRegionOnParent();
		_invalid_layout.region_on_parent = false;
		return;
	}
	if (_invalid_layout.margin) {
		assert(!style.IsRegionOnParentAuto());
		UpdateMargin(GetDisplaySize());
		_invalid_layout.margin = false;
		/* fall through to update client region */
	}
	if (_invalid_layout.client_region) {
		assert(!style.IsMarginAuto() && !style.IsRegionOnParentAuto());
		UpdateClientRegion(GetDisplayedClientSize());
		_invalid_layout.client_region = false;
		return;
	}
	if (_invalid_layout.content_layout) {
		assert(!style.IsClientRegionAuto());
		UpdateContentLayout(GetClientRegion().size);
		_invalid_layout.content_layout = false;
		return;
	}
	assert(false);
}

const Rect Wnd::UpdateRegionOnParent(Size parent_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	Rect region_on_parent = style.CalculateRegionOnParent(parent_size);
	if (_invalid_layout.margin || region_on_parent.size != GetDisplaySize() || style.IsRegionOnParentAuto()) {
		Size display_size = UpdateMarginAndClientRegion(region_on_parent.size);
		if (style.IsRegionOnParentAuto()) {
			region_on_parent.size = style.AutoResizeRegionOnParentToDisplaySize(region_on_parent.size, display_size);
		}
		UpdateMarginAndClientRegion(region_on_parent.size); // the second time, for updating scrollbar
	}
	LeaveReflowQueue();
	return region_on_parent;
}

const Size Wnd::UpdateMarginAndClientRegion(Size display_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	Margin margin_scrollbar = style.GetScrollBarMargin();
	Margin margin_without_padding = style.CalculateMarginWithoutPadding(margin_scrollbar);
	Margin margin = style.CalculateMargin(display_size, margin_without_padding);
	Size displayed_client_size = ShrinkSizeByMargin(display_size, margin);
	Rect client_region = UpdateClientRegion(displayed_client_size);
	Rect accessible_region = ExtendRegionByMargin(client_region, margin);
	display_size = ExtendSizeByMargin(client_region.size, margin);
	SetAccessibleRegion(accessible_region);
	SetClientRegion(client_region);
	SetMargin(margin_without_padding, margin);
	return display_size;
}

const Rect Wnd::UpdateClientRegion(Size displayed_client_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	Rect client_region = style.CalculateClientRegion(displayed_client_size);
	if (_invalid_layout.content_layout == true || client_region.size != GetClientRegion().size || style.IsClientRegionAuto()) {
		Rect content_region = UpdateContentLayout(client_region.size);
		if (style.IsClientRegionAuto()) {
			client_region = style.AutoResizeClientRegionToContent(client_region, content_region);
		}
		_invalid_layout.content_layout = false;
	}
	return client_region;
}

const Rect Wnd::UpdateContentLayout(Size client_size) { 
	return Rect(point_zero, client_size); 
}

void Wnd::OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
	OnClientPaint(figure_queue, GetClientRegion(), invalid_region);
}

void Wnd::OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {
	// Draw border and scroll bar.
	const WndStyle::BorderStyle& border = GetStyle().border;
	if (border._width > 0 && border._color != color_transparent) {
		figure_queue.Append(point_zero, new RoundedRectangle(display_size, border._radius, border._width, border._color));
	}
	GetStyle().scrollbar._scrollbar_resource->OnPaint(figure_queue, display_size, invalid_display_region);
}


END_NAMESPACE(WndDesign)