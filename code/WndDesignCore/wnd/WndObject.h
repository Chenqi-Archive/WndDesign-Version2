#pragma once

#include "wnd_base_interface.h"
#include "../common/uncopyable.h"
#include "../message/msg_base.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::pair;
using std::wstring;


class WndObject {
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
	void RegisterChild(WndObject& child) { wnd->AddChild(*child.wnd); child.parent = this; }
	void UnregisterChild(WndObject& child) { wnd->RemoveChild(*child.wnd); child.parent = nullptr; }
public:
	void RemoveChild(WndObject& child) { OnChildDetach(child); UnregisterChild(child); }
private:
	virtual void OnAttachToParent() {}
	virtual void OnChildDetach(WndObject& child) {}


	//// data used by parent window ////
protected:
	using Data = unsigned long long; static_assert(sizeof(Data) == 8);
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
	const Rect GetDisplayRegion() const { return wnd->GetDisplayRegion(); }
	const Size GetSize() const { return GetAccessibleRegion().size; }
	const Point GetDisplayOffset() const { return GetDisplayRegion().point; }
	const Size GetDisplaySize() const { return GetDisplayRegion().size; }
protected:
	void SetAccessibleRegion(Rect accessible_region) { wnd->SetAccessibleRegion(accessible_region); }
	const Vector SetDisplayOffset(Point display_offset) { return wnd->SetDisplayOffset(display_offset); }
	const Vector ScrollView(Vector vector) { return SetDisplayOffset(GetDisplayOffset() + vector); }  // returns the real offset shifted
private:
	virtual void OnSizeChange(Rect accessible_region) {}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) {}  // for scrolling
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) {}  // for lazy-loading


	//// layout update ////
protected:
	void JoinReflowQueue() { wnd->JoinReflowQueue(); }
	void LeaveReflowQueue() { wnd->LeaveReflowQueue(); }
protected:
	void UpdateRegionOnParent() { if (HasParent()) { GetParent()->OnChildRegionUpdate(*this); } else { LeaveReflowQueue(); } }
	static const Rect UpdateChildRegion(WndObject& child, Size parent_size) { return child.UpdateRegionOnParent(parent_size); }
	static void SetChildRegionStyle(WndObject& child, Rect parent_specified_region) { child.SetRegionStyle(parent_specified_region); }
	static void SetChildRegion(WndObject& child, Rect region_on_parent) { child.wnd->SetRegionOnParent(region_on_parent); }
	static const Rect GetChildRegion(WndObject& child) { child.wnd->GetRegionOnParent(); }
private:
	friend class ReflowQueue;
	virtual bool HasInvalidLayout() const { return false; }
	virtual bool MayRegionOnParentChange() { return false; }
	virtual void ChildRegionMayChange(WndObject& child) {}
	virtual void UpdateLayout() {}
private:
	virtual void SetRegionStyle(Rect parent_specified_region) {}
	virtual void OnChildRegionUpdate(WndObject& child) {}
	virtual const Rect UpdateRegionOnParent(Size parent_size) { return region_empty; }


	//// other window styles ////
protected:
	void TitleChanged() { if (HasParent()) { GetParent()->OnChildTitleChange(*this); } }
public:
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { return { size_min, size_max }; }
	virtual const wstring& GetTitle() const { return L""; }
private:
	virtual void OnChildTitleChange(WndObject& child) {}
	

	//// painting and composition ////
protected:
	void SetBackground(const Background& background) { wnd->SetBackground(background); }
	void AllocateLayer() { wnd->AllocateLayer(); }
	void Invalidate(Rect region) { wnd->Invalidate(region); }
	void NonClientInvalidate(Rect invalid_non_client_region) {
		if (HasParent()) { GetParent()->InvalidateChild(*this, invalid_non_client_region); }
	}
private:
	void InvalidateChild(WndObject& child, Rect child_invalid_region) {
		Rect child_region = GetChildRegion(child);
		Invalidate(child_region.Intersect(child_invalid_region + (child_region.point - point_zero)));
	}
protected:
	static void CompositeChild(const WndObject& child, FigureQueue& figure_queue, Rect parent_invalid_region) { child.wnd->Composite(figure_queue, parent_invalid_region); }
private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {}


	//// message handling ////
public:
	void SetCapture() { wnd->SetCapture(); }
	void SetFocus() { wnd->SetFocus(); }
	void ReleaseCapture() { wnd->ReleaseCapture(); }
	void ReleaseFocus() { wnd->ReleaseFocus(); }
private:
	virtual bool HitTest(Rect accessible_region, Point point) const { return true; }
	virtual bool NonClientHitTest(Size display_size, Point point) const { return false; }
	virtual const WndObject& HitTestChild(Point point) const { return *this; }
	virtual bool NonClientHandler(Msg msg, para) { return true; }
	virtual bool Handler(Msg msg, Para para) { return true; }
};


class DesktopObject : protected WndObject {
protected:
	DesktopObject(unique_ptr<IWndBase> desktop) : WndObject(std::move(desktop)) {}
	~DesktopObject() {}
public:
	WNDDESIGNCORE_API static DesktopObject& Get();

	virtual void OnChildRegionUpdate(WndObject& child) override pure;
	virtual void AddChild(WndObject& child) pure;
	virtual void MessageLoop() pure;
	virtual void Terminate() pure;
};


END_NAMESPACE(WndDesign)