#include "Wnd.h"
#include "../geometry/geometry_helper.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


Wnd::Wnd(unique_ptr<Style> style) : 
	_style(std::move(style)),
	_margin_without_padding(),
	_margin(),
	_client_region(),
	_invalid_layout({ true, true, true, true }),
	_mouse_capture_info({ ElementType::None }),
	_mouse_track_info({ ElementType::None, nullptr })
{
	if (_style == nullptr) { throw std::invalid_argument("style can't be null"); }
	SetBackground(_style->background.get());
}

Wnd::~Wnd() {}

const pair<Size, Size> Wnd::CalculateMinMaxSize(Size parent_size) {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	return style.CalculateMinMaxDisplaySize(parent_size);
}

const wstring Wnd::GetTitle() const {
	return GetStyle().title._title;
}

void Wnd::SetRegionStyle(Rect parent_specified_region) {
	Style& style = GetStyle();
	style.position.left(px(parent_specified_region.point.x));
	style.position.top(px(parent_specified_region.point.y));
	style.width.normal(px(parent_specified_region.size.width));
	style.height.normal(px(parent_specified_region.size.height));
	RegionOnParentChanged();
}

bool Wnd::IsScrollable() const { 
	return GetDisplaySize() == ExtendSizeByMargin(GetClientSize(), _margin) ? false : true;
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
		Scrollbar& scrollbar = GetScrollbar(); bool has_margin = scrollbar.HasMargin();
		UpdateMarginAndClientRegion(display_size);
		if (bool scrollbar_margin_changed = has_margin ^ scrollbar.HasMargin()) {
			UpdateMarginAndClientRegion(display_size);
		}
	}
	if (_invalid_layout.client_region) {
		assert(!style.IsMarginAuto() && !style.IsRegionOnParentAuto());
		UpdateClientRegion(ShrinkSizeByMargin(GetDisplaySize(), _margin));
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
		Scrollbar& scrollbar = GetScrollbar(); bool has_margin = scrollbar.HasMargin();
		Size display_size = UpdateMarginAndClientRegion(region_on_parent.size);
		if (is_region_on_parent_auto) {
			region_on_parent.size = style.AutoResizeRegionOnParentToDisplaySize(parent_size, region_on_parent.size, display_size);
		}
		UpdateScrollbar(GetAccessibleRegion(), Rect(point_zero + GetDisplayOffset(), region_on_parent.size));
		if (bool scrollbar_margin_changed = has_margin ^ scrollbar.HasMargin()) {
			UpdateMarginAndClientRegion(display_size);
		}
	}
	_invalid_layout.region_on_parent = false;
	return region_on_parent;
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
	_client_region = client_region; 
	// offset client region so that accessible region's origin will be at (0,0)
	SetAccessibleRegion(ExtendRegionByMargin(_client_region + GetClientOffset(), _margin));
	_invalid_layout.client_region = false;
}

void Wnd::OnDisplayRegionChange(Rect accessible_region, Rect display_region) {
	accessible_region = ShrinkRegionByMargin(accessible_region, _margin_without_padding);
	display_region = ShrinkRegionByMargin(display_region, _margin_without_padding);
	GetScrollbar().Update(
		accessible_region.size,
		display_region - (accessible_region.point - point_zero)
	);
}

void Wnd::OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
	Vector offset = figure_queue.PushOffset(GetClientOffset());
	OnClientPaint(figure_queue, GetClientRegion(), invalid_region - GetClientOffset());
	figure_queue.PopOffset(offset);
}

void Wnd::OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {
	// Draw border and scroll bar.
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	Rect display_region_without_border = style.GetDisplayRegionWithoutBorder(display_size);
	if (style.HasBorder() && !display_region_without_border.Contains(invalid_display_region)) {
		figure_queue.Append(point_zero, style.GetBorder(display_size));
	}
	if (style.HasScrollbar()) {
		Vector offset = figure_queue.PushOffset(display_region_without_border.point - point_zero);
		style.scrollbar._resource->OnPaint(figure_queue, display_region_without_border.size);
		figure_queue.PopOffset(offset);
	}
}

void Wnd::MouseCaptureInfo::Update(Wnd& wnd, ElementType type) {
	if (_type == type) { return; }
	if (_type == ElementType::None) {
		wnd.SetCapture();
	} else {
		wnd.NotifyElement(_type, Msg::LoseCapture, nullmsg);
	}
	_type = type;
}

void Wnd::MouseTrackInfo::Update(Wnd& wnd, ElementType type) {
	if (IsChild()) { _child->NonClientHandler(Msg::MouseLeave, nullmsg); _child = nullptr; }
	if (_type == type) { return; }
	wnd.NotifyElement(_type, Msg::MouseLeave, nullmsg);
	_type = type;
	wnd.NotifyElement(_type, Msg::MouseEnter, nullmsg);
}

void Wnd::MouseTrackInfo::Update(Wnd& wnd, WndObject& child) {
	Update(wnd, ElementType::None);
	_child = &child;
	_child->NonClientHandler(Msg::MouseEnter, nullmsg);
}

bool Wnd::NonClientHitTest(Size display_size, Point point) const {
	const StyleCalculator& style = GetStyleCalculator(GetStyle());
	return style.IsPointInside(display_size, point);
}

bool Wnd::NonClientHandler(Msg msg, Para para) {
	if (IsMouseMsg(msg)) {
		MouseMsg& mouse_msg = GetMouseMsg(para);
		Size display_size = GetDisplaySize();
		Rect display_region_without_border = GetStyleCalculator(GetStyle()).GetDisplayRegionWithoutBorder(display_size);
		Vector display_region_to_client_offset = GetDisplayOffset() - GetClientOffset();
		// Find the message receiver.
		do {
			// Send to captured element if has.
			if (!_mouse_capture_info.IsNull()) { 
				_mouse_track_info.Update(*this, _mouse_capture_info._type); break;
			}
			// Hit test border.
			if (GetStyleCalculator(GetStyle()).HitTestBorder(display_size, mouse_msg.point)) {
				_mouse_track_info.Update(*this, ElementType::Border); break;
			}
			// Hit test scrollbar.
			if (GetScrollbar().HitTest(display_region_without_border.size, mouse_msg.point - (display_region_without_border.point - point_zero))) {
				_mouse_track_info.Update(*this, ElementType::Scrollbar); break;
			}
			// Hit test client region.
			// First convert point on display region to point on client region.
			HitTestInfo hit_test_info = ClientHitTest(GetClientSize(), GetMouseMsg(para).point + display_region_to_client_offset);
			if (hit_test_info.child == nullptr) {
				_mouse_track_info.Update(*this, ElementType::Client); break;
			}
			// Send to child window who was hit.
			_mouse_track_info.Update(*this, *hit_test_info.child);
			GetMouseMsg(para).point = hit_test_info.point;
			return _mouse_track_info._child->NonClientHandler(msg, para);
		} while (false);

		// Send to border, scrollbar or client region.
		switch (_mouse_track_info._type) {
		case ElementType::Border: 
			GetBorderResizer().Handler(*this, msg, para); return true;
		case ElementType::Scrollbar: 
			mouse_msg.point -= display_region_without_border.point - point_zero; 
			GetScrollbar().Handler(*this, msg, para); return true;
		case ElementType::Client:
			mouse_msg.point -= display_region_to_client_offset;
			return Handler(msg, para);
		}
		assert(false); 
	}
	if (msg == Msg::MouseEnter) { return true; }
	if (msg == Msg::LoseCapture) { LoseCapture(); return true; }
	if (msg == Msg::MouseLeave) { MouseLeave(); return true; }
	if (IsKeyboardMsg(msg) || msg == Msg::LoseFocus) { return Handler(msg, para); }
	assert(false); return true; // never reached, there are no more message types
}

bool Wnd::Handler(Msg msg, Para para) {
	if (msg == Msg::MouseEnter) {
		SetCursor(GetStyle().cursor._cursor);
		return true;
	}
	if ((msg == Msg::MouseWheel || msg == Msg::MouseWheelHorizontal) && IsScrollable()) {
		Vector scroll_offset = vector_zero;
		if (msg == Msg::MouseWheel) { scroll_offset.y -= GetMouseMsg(para).wheel_delta; }
		if (msg == Msg::MouseWheelHorizontal) { scroll_offset.x -= GetMouseMsg(para).wheel_delta; }
		SetDisplayOffset(GetDisplayOffset() + scroll_offset);
		return true;
	}
	return true;
}


END_NAMESPACE(WndDesign)