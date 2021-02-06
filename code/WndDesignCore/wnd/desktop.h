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

private:
	friend class RedrawQueue;
	list_iterator<ref_ptr<WndBase>> _redraw_queue_index;
private:
	void JoinRedrawQueue();
	void LeaveRedrawQueue();

public:
	void UpdateInvalidRegion();
	void Present();

private:
	void SetCapture();
	void SetFocus();
	void ReleaseCapture();
	void ReleaseFocus();

public:
	/* callback by WndProc */
	void LoseCapture() { _wnd.ChildLoseCapture(); }
	void LoseFocus() { _wnd.ChildLoseFocus(); }
	void Invalidate(Rect region) { _wnd.Invalidate(region);	}
	void ReceiveMessage(Msg msg, Para para) const { _wnd.DispatchMessage(msg, para); }
	void SetRegion(Rect region);
	const Vector OffsetFromParent() const { return _wnd.OffsetFromParent(); }
	const pair<Size, Size> CalculateMinMaxSize();
};


class DesktopObjectImpl : public DesktopObject {
private:
	friend class DesktopBase;
	list<DesktopWndFrame> _child_wnds;

public:
	DesktopObjectImpl();
	~DesktopObjectImpl() {}

	virtual const Rect CalculateRegionOnParent(Size parent_size);

	virtual void AddChild(WndObject& child) override;
	virtual void OnChildDetach(WndObject& child) override;
	virtual void OnChildRegionUpdate(WndObject& child) override;

	static void SetChildFrame(WndObject& child, DesktopWndFrame& frame) {
		SetChildData(child, &frame);
	}
	static DesktopWndFrame& GetChildFrame(WndObject& child) {
		return *GetChildData<DesktopWndFrame*>(child);
	}

	void AddChild(WndBase& child, WndObject& child_object);

	void UpdateChildInvalidRegion(WndObject& child) { GetChildFrame(child).UpdateInvalidRegion(); }
	void SetChildCapture(WndObject& child) { GetChildFrame(child).SetCapture(); }
	void SetChildFocus(WndObject& child) { GetChildFrame(child).SetFocus(); }

	virtual void MessageLoop() override;
	virtual void Terminate() override;

	std::pair<HANDLE, const Point> ConvertPointToDesktopWndPoint(WndObject& wnd, Point point) const;
};


class DesktopBase : public WndBase {
public:
	DesktopBase(DesktopObjectImpl& desktop_object);
	~DesktopBase() {}
private:
	DesktopObjectImpl& GetObject() const { return static_cast<DesktopObjectImpl&>(_object); }
private:
	virtual const Rect GetCachedRegion() const override;
private:
	virtual void AddChild(IWndBase& child_wnd) override;
	virtual void Invalidate(WndBase& child) override { GetObject().UpdateChildInvalidRegion(child._object); }
	virtual void SetChildCapture(WndBase& child) override { GetObject().SetChildCapture(child._object); }
	virtual void SetChildFocus(WndBase& child) override { GetObject().SetChildFocus(child._object); }
	virtual void ReleaseCapture() override;
	virtual void ReleaseFocus() override;
};


END_NAMESPACE(WndDesign)