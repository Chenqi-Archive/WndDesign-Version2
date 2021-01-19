#pragma once

#include "wnd_base.h"
#include "../system/directx/d2d_api_window.h"

#include <Windows.h>  // move to *.cpp later


BEGIN_NAMESPACE(WndDesign)


class DesktopWnd : public WndBase {
private:
	HANDLE _hwnd;
	WindowResource _window;


private:
	DesktopWnd(HANDLE hwnd) : WndBase(), _hwnd(hwnd), _window(hwnd) {}

	void OnResize(Size size) { _window.OnResize(size); }

private:
	ref_ptr<WndBase> _child;

private:
	void JoinRedrawQueue();
	void LeaveRedrawQueue();

	const Rect GetCachedRegion() const { return Rect(point_zero, size); }


	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region) {
		
	}

	void Invalidate(Region&& region) {
		JoinDesktopRedrawQueue;
	}

	void UpdateInvalidRegion() {
		auto [bounding_region, regions] = _invalid_region.GetRect();

		FigureQueue figure_queue;
		uint group_index = figure_queue.BeginGroup(vector_zero, bounding_region);
		figure_queue.Append(bounding_region.point - point_zero, new BackgroundFigure(_background, bounding_region, true));

		_object.OnPaint(figure_queue, _accessible_region, bounding_region);  /* no object */

		figure_queue.EndGroup(group_index);

		Target& target = _window.GetTarget();
		for (auto& region : regions) {
			target.DrawFigureQueue(figure_queue, vector_zero, region);
		}

		_window.Present(std::move(regions));

		_invalid_region.Clear();  // Clear invalid region.
	}

private:
	virtual void SetChildCapture(WndBase& child) override { ::SetCapture(static_cast<HWND>(_hwnd)); }
	virtual void SetChildFocus(WndBase& child) override { ::SetFocus(static_cast<HWND>(_hwnd)); }
	virtual void ReleaseCapture() override { ::ReleaseCapture(); }
	virtual void ReleaseFocus() override{ ::SetFocus(NULL); }
};


END_NAMESPACE(WndDesign)