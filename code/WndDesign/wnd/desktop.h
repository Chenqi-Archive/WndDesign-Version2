#pragma once

#include "wnd_base.h"
#include "../system/directx/d2d_api_window.h"


BEGIN_NAMESPACE(WndDesign)

using std::pair;


class DesktopWndFrame {
private:
	WndBase& _wnd;
	HANDLE _hwnd;
	WindowResource _resource;
public:
	DesktopWndFrame(WndBase& wnd, HANDLE hwnd) :_wnd(wnd), _hwnd(hwnd), _resource(_hwnd) {}
	~DesktopWndFrame() {}
	void OnResize(Size size) { _resource.OnResize(size); }
	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region) {
		_resource.GetTarget().DrawFigureQueue(figure_queue, vector_zero, bounding_region);
	}
	void SetRegion(Rect region);

	void SetCapture();
	void SetFocus();
	void ReleaseCapture();
	void ReleaseFocus();

	/* called by WndProc */
	void RegionUpdated(Rect region);
	void SetRegion(Rect region);
	void ReceiveMessage(Msg msg, Para para) const;
	void LoseCapture() {_wnd.ChildLoseCapture();}
	void LoseFocus() {_wnd.ChildLoseFocus();}
	const pair<Size, Size> CalculateMinMaxSize();
};


class Desktop : public WndBase {
public:
	Desktop(WndObject& desktop_object) : WndBase(desktop_object) {}
	~Desktop() {}

private:
	list<DesktopWndFrame> _child_wnds;

	static inline DesktopWndFrame& GetChildFrame(WndBase& child) {
		static_assert(sizeof(list<DesktopWndFrame>::iterator) == sizeof(list<ref_ptr<WndBase>>::iterator));
		return *reinterpret_cast<list<DesktopWndFrame>::iterator&>(child._index_on_parent);
	}
	static inline void SetChildFrame(WndBase& child, const list<DesktopWndFrame>::iterator& frame) {
		static_assert(sizeof(list<DesktopWndFrame>::iterator) == sizeof(list<ref_ptr<WndBase>>::iterator));
		reinterpret_cast<list<DesktopWndFrame>::iterator&>(child._index_on_parent) = frame;
	}

	virtual void AddChild(IWndBase& child_wnd) override;
	virtual void RemoveChild(IWndBase& child_wnd) override;

private:
	void JoinRedrawQueue();
	void LeaveRedrawQueue();

	const Rect GetCachedRegion() const { return Rect(point_zero, size); }


	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region) {

	}

	virtual void Invalidate(WndBase& child) override {
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
	virtual void ReleaseFocus() override { ::SetFocus(NULL); }
};


END_NAMESPACE(WndDesign)