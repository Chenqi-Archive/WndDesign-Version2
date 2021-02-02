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

const Margin Wnd::CalculateNonClientMargin() {
	WndStyle& style = GetStyle();
	Margin non_client_region;
	non_client_region.left = style.border._width + style.padding._left;
	non_client_region.top = style.border._width;
	non_client_region.right = style.border._width;
	non_client_region.bottom = style.border._width;
	return non_client_region;
}

const Rect Wnd::GetClientRegion(Rect accessible_region) const {
	return ShrinkRegionByMargin(accessible_region, _non_client_margin);
}

void Wnd::OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
	OnClientPaint(figure_queue, GetClientRegion(accessible_region), invalid_region);
}

void Wnd::OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {
	// draw border, scroll bar, etc

	figure_queue.
}


END_NAMESPACE(WndDesign)