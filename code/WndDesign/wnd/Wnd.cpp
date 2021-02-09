#include "Wnd.h"
#include "../geometry/geometry_helper.h"
#include "../style/style_helper.h"
#include "../figure/figure_queue.h"
#include "../message/message.h"


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
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	if (_invalid_layout.region_on_parent) {
		WndObject::UpdateRegionOnParent();
	}
	if (_invalid_layout.margin) {
		assert(!style.IsRegionOnParentAuto());
		Size display_size = GetDisplaySize();
		UpdateMarginAndClientRegion(display_size);
		bool scrollbar_margin_changed = UpdateScrollbar();
		if (scrollbar_margin_changed) {
			UpdateMarginAndClientRegion(display_size);
			UpdateScrollbar();
		}
	}
	if (_invalid_layout.client_region) {
		assert(!style.IsMarginAuto() && !style.IsRegionOnParentAuto());
		UpdateClientRegion(ShrinkSizeByMargin(GetDisplaySize(), _margin));
		UpdateScrollbar();
	}
	if (_invalid_layout.content_layout) {
		assert(!style.IsClientRegionAuto());
		UpdateContentLayout(GetClientRegion().size);
		_invalid_layout.content_layout = false;
	}
}

const Rect Wnd::UpdateRegionOnParent(Size parent_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	Rect region_on_parent = style.CalculateRegionOnParent(parent_size);
	bool is_region_on_parent_auto = style.IsRegionOnParentAuto();
	if (_invalid_layout.margin || region_on_parent.size != GetDisplaySize() || is_region_on_parent_auto) {
		Size display_size = UpdateMarginAndClientRegion(region_on_parent.size);
		if (is_region_on_parent_auto) {
			region_on_parent.size = style.AutoResizeRegionOnParentToDisplaySize(region_on_parent.size, display_size);
		}
		bool scrollbar_margin_changed = UpdateScrollbar();
		if (scrollbar_margin_changed) {
			UpdateMarginAndClientRegion(region_on_parent.size);
			UpdateScrollbar();
		}
	}
	_invalid_layout.region_on_parent = false;
	return region_on_parent;
}

bool Wnd::UpdateScrollbar() {
	return GetStyleCalculator(GetStyle()).UpdateScrollbar(
		ShrinkRegionByMargin(GetAccessibleRegion(), _margin_without_padding),
		ShrinkRegionByMargin(GetDisplayRegion(), _margin_without_padding)
	);
}

const Size Wnd::UpdateMarginAndClientRegion(Size display_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	_margin_without_padding = style.CalculateBorderMargin() + style.GetScrollbarMargin();
	_margin = style.CalculatePaddingMargin(display_size) + _margin_without_padding;
	_invalid_layout.margin = false;
	UpdateClientRegion(ShrinkSizeByMargin(display_size, _margin));
	display_size = ExtendSizeByMargin(_client_region.size, _margin);
	return display_size;
}

void Wnd::UpdateClientRegion(Size displayed_client_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	Rect client_region = style.CalculateClientRegion(displayed_client_size);
	bool is_client_auto = style.IsClientRegionAuto();
	if (_invalid_layout.content_layout == true || client_region.size != GetClientRegion().size || is_client_auto) {
		_invalid_layout.content_layout = false;
		Rect content_region = UpdateContentLayout(client_region.size);
		if (is_client_auto) { 
			client_region = style.AutoResizeClientRegionToContent(client_region, content_region); 
		}
	}
	_client_region = client_region + GetClientOffset(); // offset client region so that accessible region's origin will be at (0,0)
	SetAccessibleRegion(ExtendRegionByMargin(_client_region, _margin));
	_invalid_layout.client_region = false;
}

void Wnd::OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
	OnClientPaint(figure_queue, GetClientRegion(), invalid_region);
}

void Wnd::OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {
	// Draw border and scroll bar.
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	Rect display_region_without_border = style.GetDisplayRegionWithoutBorder(display_size);
	if (style.HasBorder() && !display_region_without_border.Contains(invalid_display_region)) {
		figure_queue.Append(point_zero, new RoundedRectangle(style.GetBorder(display_size)));
	}
	if (style.HasScrollbar()) {
		uint group_begin = figure_queue.BeginGroup(display_region_without_border.point - point_zero, display_region_without_border);
		style.scrollbar._scrollbar_resource->OnPaint(figure_queue, display_region_without_border.size);
		figure_queue.EndGroup(group_begin);
	}
}

bool Wnd::NonClientHitTest(Size display_size, Point point) const { 
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	return style.IsPointInside(display_size, point);
}

bool Wnd::NonClientHandler(Msg msg, Para para) {
	// Hit test border and scrollbar.

	return WndObject::NonClientHandler(msg, para);
}

bool Wnd::Handler(Msg msg, Para para) {
	if (IsMouseMsg(msg)) {
		MouseMsg& mouse_msg = GetMouseMsg(para);
		mouse_msg.point = mouse_msg.point - GetClientOffset();
	}
	if (msg == Msg::MouseEnter) {
		SetCursor(GetStyle().cursor._cursor);
	}
	return ClientHandler(msg, para);
}


END_NAMESPACE(WndDesign)