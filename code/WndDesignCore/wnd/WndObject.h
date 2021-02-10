#pragma once

#include "wnd_base_interface.h"
#include "../common/uncopyable.h"
#include "../layer/figure_queue.h"
#include "../message/message.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::pair;
using std::wstring;


class WndObject : Uncopyable {
private:
	friend class WndBase;
	unique_ptr<IWndBase> wnd;
private:
	friend class DesktopObject;
	WndObject(unique_ptr<IWndBase> desktop) : wnd(std::move(desktop)), parent(nullptr) {}
protected:
	WndObject() : wnd(IWndBase::Create(*this)), parent(nullptr) {}
	~WndObject() {}


	//// child and parent window relation ////
private:
	ref_ptr<WndObject> parent;
public:
	ref_ptr<WndObject> GetParent() const { return parent; }
	bool HasParent() const { return GetParent() != nullptr; }
	bool IsMyChild(WndObject& child) const { return child.GetParent() == this; }
protected:
	void RegisterChild(WndObject& child) { 
		assert(child.GetParent() != this);
		child.parent = this; wnd->AddChild(*child.wnd);
	}
	void UnregisterChild(WndObject& child) { 
		assert(child.GetParent() == this);
		wnd->RemoveChild(*child.wnd); child.parent = nullptr;
	}
public:
	void RemoveChild(WndObject& child) { OnChildDetach(child); UnregisterChild(child); }
private:
	virtual void OnAttachToParent() {}
	virtual void OnChildDetach(WndObject& child) {}


	//// data used by parent window ////
protected:
	using Data = ulonglong; static_assert(sizeof(Data) == 8);
private:
	Data data = 0;
protected:
	template<class T> static void SetChildData(WndObject& child, T data) { 
		static_assert(sizeof(Data) == sizeof(T));
		child.data = reinterpret_cast<Data>(data);
	}
	template<class T> static T GetChildData(WndObject& child) {
		static_assert(sizeof(Data) == sizeof(T));
		return reinterpret_cast<T>(child.data);
	}


	//// window region ////
public:
	const Rect GetAccessibleRegion() const { return wnd->GetAccessibleRegion(); }
	const Size GetSize() const { return GetAccessibleRegion().size; }
	const Rect GetDisplayRegion() const { return wnd->GetDisplayRegion(); }
	const Size GetDisplaySize() const { return GetDisplayRegion().size; }
	const Vector GetDisplayOffset() const { return GetDisplayRegion().point - point_zero; }
public:
	const Point ConvertPointToNonClientPoint(Point point) const { return point - GetDisplayOffset(); }
	const Point ConvertNonClientPointToParentPoint(Point point) const { return point + (wnd->GetRegionOnParent().point - point_zero); }
protected:
	void SetAccessibleRegion(Rect accessible_region) { wnd->SetAccessibleRegion(accessible_region); }
	const Vector SetDisplayOffset(Vector display_offset) { return wnd->SetDisplayOffset(point_zero + display_offset); }
	const Vector ScrollView(Vector vector) { return SetDisplayOffset(GetDisplayOffset() + vector); }  // returns the real offset shifted
private:
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) {}  // for scrolling
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) {}  // for lazy-loading


	//// layout update ////
protected:
	void InvalidateLayout() { wnd->InvalidateLayout(); }
	void UpdateRegionOnParent() { if (HasParent()) { GetParent()->OnChildRegionUpdate(*this); } }
protected:
	static const Rect UpdateChildRegion(WndObject& child, Size parent_size) { return child.UpdateRegionOnParent(parent_size); }
	static void SetChildRegionStyle(WndObject& child, Rect parent_specified_region) { child.SetRegionStyle(parent_specified_region); }
	static void SetChildRegion(WndObject& child, Rect region_on_parent) { child.wnd->SetRegionOnParent(region_on_parent); }
	static const Rect GetChildRegion(WndObject& child) { child.wnd->GetRegionOnParent(); }
private:
	virtual bool HasInvalidLayout() const { return false; }
	virtual bool MayRegionOnParentChange() { return false; }
	virtual void ChildRegionMayChange(WndObject& child) {}
	virtual void UpdateLayout() {}
private:
	virtual void SetRegionStyle(Rect parent_specified_region) {}
	virtual void OnChildRegionUpdate(WndObject& child) {}
	virtual const Rect UpdateRegionOnParent(Size parent_size) { return region_empty; }


	//// other window styles ////
public:
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { return { size_min, size_max }; }
	virtual const wstring GetTitle() const { return L""; }
protected:
	void TitleChanged() { if (HasParent()) { GetParent()->OnChildTitleChange(*this); } }
private:
	virtual void OnChildTitleChange(WndObject& child) {}
	

	//// painting and composition ////
protected:
	void SetBackground(const Background& background) { wnd->SetBackground(background); }
	void AllocateLayer() { wnd->AllocateLayer(); }
	void Invalidate(Rect invalidate_client_region) { wnd->Invalidate(invalidate_client_region); }
	void NonClientInvalidate(Rect invalid_non_client_region) {
		if (HasParent()) { GetParent()->InvalidateChild(*this, invalid_non_client_region); }
	}
protected:
	static void CompositeChild(const WndObject& child, FigureQueue& figure_queue, Rect parent_invalid_region) { 
		child.wnd->Composite(figure_queue, parent_invalid_region); 
	}
private:
	void InvalidateChild(WndObject& child, Rect child_invalid_region) { wnd->InvalidateChild(*child.wnd, child_invalid_region); }
private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {}


	//// message handling ////
public:
	void SetCapture() { wnd->SetCapture(); }
	void SetFocus() { wnd->SetFocus(); }
	void ReleaseCapture() { wnd->ReleaseCapture(); }
	void ReleaseFocus() { wnd->ReleaseFocus(); }
protected:
	static bool HitTestChild(const WndObject& child, Point point) { return child.NonClientHitTest(child.GetDisplaySize(), point); }
	bool SendChildMessage(WndObject& child, Msg msg, Para para) { return wnd->SendChildMessage(*child.wnd, msg, para); }
private:
	virtual bool NonClientHitTest(Size display_size, Point point) const { return true; }
protected:
	virtual bool NonClientHandler(Msg msg, Para para) {
		if (IsMouseMsg(msg)) { MouseMsg& mouse_msg = GetMouseMsg(para); mouse_msg.point = mouse_msg.point + GetDisplayOffset(); }
		return Handler(msg, para);
	}
	virtual bool Handler(Msg msg, Para para) { return true; }
};


class ABSTRACT_BASE DesktopObject : protected WndObject {
protected:
	DesktopObject(unique_ptr<IWndBase> desktop) : WndObject(std::move(desktop)) {}
	~DesktopObject() {}

public:
	WNDDESIGNCORE_API static DesktopObject& Get();

	void AddChild(WndObject& child) { RegisterChild(child); }

	virtual void OnChildDetach(WndObject& child) override pure {}
	virtual void OnChildRegionUpdate(WndObject& child) override pure {}
	virtual void OnChildTitleChange(WndObject& child) override pure {}

	virtual void MessageLoop() pure;
	virtual void Terminate() pure;
};


END_NAMESPACE(WndDesign)