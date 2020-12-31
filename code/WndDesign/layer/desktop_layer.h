#pragma once

#include "figure_queue.h"
#include "background.h"
#include "../common/reference_wrapper.h"
#include "../system/directx/d2d_api_window.h"


BEGIN_NAMESPACE(WndDesign)


class DesktopLayer {
private:
	WindowResource _window;
	reference_wrapper<const Background> _background;

public:
	DesktopLayer(HANDLE hwnd, const Background& background) :
		_window(hwnd), _background(background), _dirty_regions() {
	}

	void OnResize(Size size) { _window.OnResize(size); }

	const Size GetSize() { return _window.GetSize(); }
	const Background& GetBackground() const { return _background; }

private:
	vector<Rect> _dirty_regions;
	void AddDirtyRegion(const Rect& region);

public:
	void ClearRegion(Rect region);	// Clear the region with background.
	void DrawFigureQueue(const FigureQueue& figure_queue, Vector position_offset, Rect bounding_region);
	void CommitDraw();
};


END_NAMESPACE(WndDesign)