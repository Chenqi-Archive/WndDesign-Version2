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
	friend class DesktopObjectImpl;
	unique_ptr<IWndBase> wnd;
private:
	friend class DesktopObject;
	WndObject(unique_ptr<IWndBase> desktop) : wnd(std::move(desktop)), parent(nullptr) {}
protected:
	WndObject() : wnd(IWndBase::Create(*this)), parent(nullptr) {}
	virtual ~WndObject() {}


	//// child and parent window relation ////
private:
	ref_ptr<WndObject> parent;
public:
	bool HasParent() const { return GetParent() != nullptr; }
	ref_ptr<WndObject> GetParent() const { return parent; }
protected:
	bool IsMyChild(WndObject& child) const { 
		return child.GetParent() == this; 
	}
	void RegisterChild(WndObject& child) {
		if (IsMyChild(child)) { throw std::invalid_argument("child already registered"); }
		wnd->AddChild(*child.wnd); child.parent = this; child.OnAttachToParent();
	}
private:
	void UnregisterChild(WndObject& child) {
		wnd->RemoveChild(*child.wnd); child.parent = nullptr; child.NotifyDesktopWhenDetached();
	}
	void NotifyDesktopWhenDetached();  // defined below
public:
	void RemoveChild(WndObject& child) { 
		if (!IsMyChild(child)) { throw std::invalid_argument("child not registered"); }
		OnChildDetach(child); UnregisterChild(child); 
	}
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
		child.data = *reinterpret_cast<Data*>(&data);
	}
	template<class T> static T GetChildData(WndObject& child) {
		static_assert(sizeof(Data) == sizeof(T));
		return *reinterpret_cast<T*>(&child.data);
	}


	//// window region ////
protected:
	const Rect GetAccessibleRegion() const { return wnd->GetAccessibleRegion(); }
	const Rect GetDisplayRegion() const { return wnd->GetDisplayRegion(); }
public:
	const Size GetSize() const { return GetAccessibleRegion().size; }
	const Size GetDisplaySize() const { return GetDisplayRegion().size; }
	const Vector GetDisplayOffset() const { return GetDisplayRegion().point - point_zero; }
	const Point ConvertPointToNonClientPoint(Point point) const { return point - GetDisplayOffset(); }
	const Point ConvertNonClientPointToParentPoint(Point point) const { return point + (wnd->GetRegionOnParent().point - point_zero); }
protected:
	void SetAccessibleRegion(Rect accessible_region) { wnd->SetAccessibleRegion(accessible_region); }
	const Vector SetDisplayOffset(Vector display_offset) { return wnd->SetDisplayOffset(display_offset); }
private:
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) {}  // for scrolling
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) {}  // for lazy-loading


	//// layout update ////
protected:
	void InvalidateLayout() { wnd->InvalidateLayout(); }
	void UpdateRegionOnParent() { if (HasParent()) { GetParent()->OnChildRegionUpdate(*this); } }
protected:
	static const Rect UpdateChildRegion(WndObject& child, Size parent_size) { return child.UpdateRegionOnParent(parent_size); }
	static void SetChildRegionStyle(WndObject& child, Rect child_region, Size client_size) { child.SetRegionStyle(child_region, client_size); }
	static void SetChildRegion(WndObject& child, Rect region_on_parent) { child.wnd->SetRegionOnParent(region_on_parent); }
	static const Rect GetChildRegion(WndObject& child) { return child.wnd->GetRegionOnParent(); }
private:
	virtual bool MayRegionOnParentChange() { return false; }
	virtual void ChildRegionMayChange(WndObject& child) {}
	virtual void UpdateLayout() { UpdateRegionOnParent(); }
private:
	virtual void SetRegionStyle(Rect parent_specified_region, Size parent_size) {}
	virtual void OnChildRegionUpdate(WndObject& child) {}
	virtual const Rect UpdateRegionOnParent(Size parent_size) { return region_empty; }


	//// other window styles ////
public:
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { return { size_min, size_max }; }
	virtual const wstring GetTitle() const { return L""; }
	virtual const CompositeEffect GetCompositeEffect() const { return {}; }
protected:
	void TitleChanged() { if (HasParent()) { GetParent()->OnChildTitleChange(*this); } }
	void CompositeEffectChanged() { if (HasParent()) { GetParent()->OnChildCompositeEffectChange(*this); } }
private:
	virtual void OnChildTitleChange(WndObject& child) {}
	virtual void OnChildCompositeEffectChange(WndObject& child) { InvalidateChild(child, region_infinite); }


	//// painting and composition ////
protected:
	void SetBackground(const Background& background) { wnd->SetBackground(background); Invalidate(region_infinite); }
	void AllocateLayer() { wnd->AllocateLayer(); }
	void Invalidate(Rect invalidate_client_region) { wnd->Invalidate(invalidate_client_region); }
	void NonClientInvalidate(Rect invalid_non_client_region) {
		if (HasParent()) { GetParent()->InvalidateChild(*this, invalid_non_client_region); }
	}
protected:
	static void CompositeChild(const WndObject& child, FigureQueue& figure_queue, Rect parent_invalid_region) {
		CompositeEffect composite_effect = child.GetCompositeEffect(); if (composite_effect._opacity == 0) { return; }
		child.wnd->Composite(figure_queue, parent_invalid_region, composite_effect);
	}
private:
	void InvalidateChild(WndObject& child, Rect child_invalid_region) { wnd->InvalidateChild(*child.wnd, child_invalid_region); }
private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {}


	//// message handling ////
protected:
	void SetCapture();
	void ReleaseCapture();
	void SetFocus();
public:
	virtual bool NonClientHitTest(Size display_size, Point point) const { return true; }
	virtual void NonClientHandler(Msg msg, Para para) { 
		if (IsMouseMsg(msg)) { GetMouseMsg(para).point += GetDisplayOffset(); } return Handler(msg, para);
	}
	virtual void Handler(Msg msg, Para para) {}
};


class ABSTRACT_BASE DesktopObject : protected WndObject {
protected:
	DesktopObject(unique_ptr<IWndBase> desktop) : WndObject(std::move(desktop)) {}
	~DesktopObject() {}

public:
	WNDDESIGNCORE_API static DesktopObject& Get();

public:
	virtual void AddChild(WndObject& child, uint win32_extended_style = 0) pure;
	void RemoveChild(WndObject& child) { if (IsMyChild(child)) { WndObject::RemoveChild(child); } }

public:
	virtual void CommitReflowQueue() pure;
	virtual void CommitRedrawQueue() pure;

private:
	friend class WndObject;
	virtual void OnWndDetach(WndObject& wnd) pure;
	virtual void SetCapture(WndObject& wnd) pure;
	virtual void ReleaseCapture() pure;
	virtual void SetFocus(WndObject& wnd) pure;
public:
	virtual void MessageLoop() pure;
	virtual void Terminate() pure;
};

extern DesktopObject& desktop;  // initialized in WndObject.cpp (both in dll and in lib)


inline void WndObject::NotifyDesktopWhenDetached() { desktop.OnWndDetach(*this); }
inline void WndObject::SetCapture() { desktop.SetCapture(*this); }
inline void WndObject::ReleaseCapture() { desktop.ReleaseCapture(); }
inline void WndObject::SetFocus() { desktop.SetFocus(*this); }


END_NAMESPACE(WndDesign)