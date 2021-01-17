#pragma once

#include "figure_queue.h"
#include "background.h"
#include "../common/reference_wrapper.h"
#include "../system/directx/d2d_api_window.h"


BEGIN_NAMESPACE(WndDesign)


class DesktopLayer {
private:
	WindowResource _window;

public:
	DesktopLayer(HANDLE hwnd) :_window(hwnd) {}

	void OnResize(Size size) { _window.OnResize(size); }

	const Size GetSize() { return _window.GetSize(); }

public:
	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region) {
		_window.GetTarget().DrawFigureQueue(figure_queue, vector_zero, bounding_region);
	}
};


END_NAMESPACE(WndDesign)