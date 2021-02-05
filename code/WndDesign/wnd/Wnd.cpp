#include "Wnd.h"
#include "../figure/figure_queue.h"
#include "../geometry/geometry_helper.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


inline Wnd::Wnd(unique_ptr<Style> style) : _style(std::move(style)) {
	if (style == nullptr) { throw std::invalid_argument("style can't be null"); }
	SetBackground(_style->background.get());
}

inline Wnd::~Wnd() {}

void Wnd::OnSizeChange(Rect accessible_region) {
	// change size or position style according to accessible region.
	// also when region on parent changed.
}

const pair<Size, Size> Wnd::CalculateMinMaxSize(Size parent_size) {
	return CalculateMinMaxSizeFromStyle(*_style, parent_size);
}

const wstring& Wnd::GetTitle() const {
	return L"MainWnd";
}

bool Wnd::CheckRegionAutoChange() const {
	if (IsClientRegionAuto() && _invalid_layout.content_layout) {}
	if (IsRegionOnParentAuto() && _invalid_layout.non_client_margin) {}
	if (IsRegionOnParentAuto() && _invalid_layout.client_region) {}

	if (_invalid_layout.region_on_parent) { return true; }
	// ReflowQueue will notify parent window if region_on_parent may change.
	// if (HasParent()) { GetParent()->ChildLayoutChanged(*this); }
	return false;
}

void Wnd::OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
#error convert coordinates to client region.
	OnClientPaint(figure_queue, GetClientRegion(), invalid_region);
}

void Wnd::OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {
	// draw border, scroll bar, etc

	figure_queue.
}


END_NAMESPACE(WndDesign)