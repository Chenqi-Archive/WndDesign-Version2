#pragma once

#include "wnd_base.h"
#include "WndObject.h"
#include "../system/directx/d2d_api_window.h"


BEGIN_NAMESPACE(WndDesign)

using std::pair;


class DesktopWndFrame : public Uncopyable{
private:
	WndBase& _wnd;
	WndObject& _wnd_object;
	HANDLE _hwnd;
	WindowResource _resource;
private:
	friend class DesktopObjectImpl;
	list<DesktopWndFrame>::iterator _desktop_index;

public:
	DesktopWndFrame(WndBase& wnd, WndObject& wnd_object, HANDLE hwnd);
	~DesktopWndFrame();

public:
	void OnRegionChange(Rect region);
	void SetRegion(Rect region);
	const Vector GetRegionOffset() const { return point_zero - _wnd._region_on_parent.point; }
	const pair<Size, Size> CalculateMinMaxSize();

private:
	friend class RedrawQueue;
	list_iterator<ref_ptr<WndBase>> _redraw_queue_index;
private:
	void JoinRedrawQueue();
	void LeaveRedrawQueue();
private:
	Region _invalid_region;
public:
	void Invalidate(Region& region);
	void Invalidate(Rect region);
public:
	void UpdateInvalidRegion();
	void Present();

private:
	Point _capture_wnd_offset_from_desktop;
	ref_ptr<WndObject> _capture_wnd = nullptr; 
	ref_ptr<WndObject> _focus_wnd = nullptr;
private:
	void OnWndDetach(WndObject& wnd);
	void SetCapture(WndObject& wnd, Vector offset);
	void SetFocus(WndObject& wnd);
public:
	void LoseCapture();
	void LoseFocus();
	void ReceiveMessage(Msg msg, Para para) const;
};


class DesktopObjectImpl : public DesktopObject {
private:
	friend class DesktopBase;
	list<DesktopWndFrame> _child_wnds;

public:
	DesktopObjectImpl();
	~DesktopObjectImpl();

public:
	static void SetChildFrame(WndObject& child, DesktopWndFrame& frame) {
		SetChildData(child, &frame);
	}
	static DesktopWndFrame& GetChildFrame(WndObject& child) {
		return *GetChildData<DesktopWndFrame*>(child);
	}

	void AddChild(WndBase& child, WndObject& child_object);
	virtual void OnChildDetach(WndObject& child) override;

public:
	void SetChildRegionStyle(WndObject& child, Rect parent_specified_region);
	virtual void OnChildTitleChange(WndObject& child) override;

public:
	virtual void OnChildRegionUpdate(WndObject& child) override;

public:
	void InvalidateChild(WndObject& child, Region& child_invalid_region) { GetChildFrame(child).Invalidate(child_invalid_region); }

public:
	virtual void OnWndDetach(WndObject& wnd) override;
	virtual void SetCapture(WndObject& wnd) override;
	virtual void ReleaseCapture() override;
	virtual void SetFocus(WndObject& wnd) override;

	virtual void MessageLoop() override;
	virtual void Terminate() override;


	//// win32 helper functions ////
private:
	ref_ptr<DesktopWndFrame> GetWndFrame(WndObject& wnd) const;
	const std::pair<ref_ptr<DesktopWndFrame>, Point> ConvertWndNonClientPointToFramePoint(WndObject& wnd, Point point) const;
public:
	HANDLE GetWndHandle(WndObject& wnd) const { 
		if (auto frame = GetWndFrame(wnd); frame != nullptr) { return frame->_hwnd; }
		return nullptr;
	}
	const std::pair<HANDLE, Point> ConvertWndNonClientPointToDesktopWindowPoint(WndObject& wnd, Point point) const {
		if (auto [frame, point_on_frame] = ConvertWndNonClientPointToFramePoint(wnd, point); frame != nullptr) { return { frame->_hwnd, point_on_frame }; }
		return { nullptr, point_zero };
	}
};


class DesktopBase : public WndBase {
public:
	DesktopBase(DesktopObjectImpl& desktop_object) : WndBase(desktop_object) { _depth = 0; }
	~DesktopBase() {}
private:
	DesktopObjectImpl& GetObject() const { return static_cast<DesktopObjectImpl&>(_object); }
private:
	virtual const Rect GetCachedRegion() const override;
private:
	virtual void AddChild(IWndBase& child_wnd) override;
	virtual void InvalidateChild(WndBase& child, Region& child_invalid_region) override { GetObject().InvalidateChild(child._object, child_invalid_region); }
};


END_NAMESPACE(WndDesign)