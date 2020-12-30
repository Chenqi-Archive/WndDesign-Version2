#include "desktop_layer.h"
#include "../system/directx/directx_helper.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


inline void DesktopLayer::AddDirtyRegion(const Rect& region) {
	// If the regions do not intersects, it may be better, but more complex to seperate them.
	_dirty_region = _dirty_region.Union(region);
}

void DesktopLayer::ClearRegion(Rect region) {
	region = region.Intersect(Rect(point_zero, GetSize()));
	auto& device_context = GetD2DDeviceContext();
	_window.GetTarget().BeginDraw(region); AddDirtyRegion(region);
	GetBackground().DrawOn(region, 0xFF, static_cast<RenderTarget&>(device_context), region.point - point_zero);
}

void DesktopLayer::DrawFigureQueue(const FigureQueue& figure_queue, Vector position_offset, Rect bounding_region) {
	bounding_region = bounding_region.Intersect(Rect(point_zero, GetSize()));
	auto& device_context = GetD2DDeviceContext();
	_window.GetTarget().BeginDraw(bounding_region); AddDirtyRegion(bounding_region);
	for (auto& container : figure_queue) {
		Vector figure_offset = container.offset + position_offset;
		Rect figure_region = container.figure->GetRegion() + figure_offset;
		if (bounding_region.Intersect(figure_region).IsEmpty()) { continue; }
		container.figure->DrawOn(static_cast<RenderTarget&>(device_context), figure_offset);
	}
}

void DesktopLayer::CommitDraw() {
	if (_dirty_region.IsEmpty()) {
		_window.GetTarget().EndDraw();
	} else { // EndDraw is called inside the Present function.
		_window.Present(_dirty_region);
		_dirty_region = region_empty;
	}
}


END_NAMESPACE(WndDesign)