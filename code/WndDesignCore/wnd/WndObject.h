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
	friend class DesktopObject;
	unique_ptr<IWndBase> wnd;
	ref_ptr<WndObject> parent;

private:
	WndObject(unique_ptr<IWndBase> desktop) : wnd(std::move(desktop)), parent(nullptr) {}
protected:
	WndObject() : wnd(IWndBase::Create(*this)), parent(nullptr) {}
	~WndObject() {}

	//// child and parent window relation ////
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
	virtual void OnChildDetach(WndObject& child) {}

	//// data used by parent window ////
protected:
	using Data = unsigned long long;
	static_assert(sizeof(Data) == 8);
private:
	// data is used for parent to store information in child windows.
	Data data = 0;
protected:
	template<class T>
	static void SetChildData(WndObject& child, T data) {
		static_assert(sizeof(Data) == sizeof(T));
		child.data = reinterpret_cast<Data>(data);
	}
	template<class T>
	static T GetChildData(WndObject& child) {
		static_assert(sizeof(Data) == sizeof(T));
		return reinterpret_cast<T>(child.data);
	}

	//// window region ////
public:
	const Rect GetAccessibleRegion() const { return wnd->GetAccessibleRegion(); }
	const Size GetSize() const { return GetAccessibleRegion().size; }
	const Rect GetDisplayRegion() const { return wnd->GetDisplayRegion(); }
	const Point GetDisplayOffset() const { return GetDisplayRegion().point; }
	const Vector SetDisplayOffset(Point display_offset) { return wnd->SetDisplayOffset(display_offset); }
	const Vector ScrollView(Vector vector) { return SetDisplayOffset(GetDisplayOffset() + vector); }  // returns the real vector shifted
	const Rect GetRegionOnParnet() const { return wnd->GetRegionOnParent(); }
	const Point ConvertPointToParentPoint(Point point) const { return point + (GetRegionOnParnet().point - point_zero); }
private:
	void SetAccessibleRegion(Rect accessible_region) { wnd->SetAccessibleRegion(accessible_region); }
private:
	virtual void OnSizeChange(Rect accessible_region) {}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) {}  // for scrolling
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) {}  // for lazy-loading

	//// region computing ////
protected:
	void RegionChanged() { if (auto parent = GetParent(); parent != nullptr) { parent->OnChildRegionChange(*this); } }
protected:
	/* called by parent window to get and set the child window's region */
	static void SetChildRegion(WndObject& child, Rect region_on_parent) { child.wnd->SetRegionOnParent(region_on_parent); }
	static const Rect GetChildRegion(WndObject& child, Size parent_size) {
		Rect region_on_parent = child.CalculateRegionOnParent(parent_size);
		Rect accessible_region = child.CalculateAccessibleRegion(parent_size, region_on_parent.size);
		child.SetAccessibleRegion(accessible_region);
		return region_on_parent;
	}
private:
	virtual void OnChildRegionChange(WndObject& child) {}
	virtual const Rect CalculateRegionOnParent(Size parent_size) { return region_empty; }
	virtual const Rect CalculateAccessibleRegion(Size parent_size, Size display_region_size) { return Rect(point_zero, display_region_size); }

	//// other window styles ////
public:
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { return { size_min, size_max }; }
	virtual const wstring& GetTitle() const { return L""; }
	
	//// painting and composition ////
public:
	void SetBackground(const Background& background) { wnd->SetBackground(background); }
	void AllocateLayer() { wnd->AllocateLayer(); }
	void Invalidate(Rect region) { wnd->Invalidate(region); }
protected:
	/* called by parent window when parent window is painting */
	void Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const { wnd->Composite(figure_queue, parent_invalid_region); }
	/* draw anything to the invalid region except for background */
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}

	//// message handling ////
public:
	void SetCapture() { wnd->SetCapture(); }
	void SetFocus() { wnd->SetFocus(); }
	void ReleaseCapture() { wnd->ReleaseCapture(); }
	void ReleaseFocus() { wnd->ReleaseFocus(); }
private:
	virtual const WndObject& HitTestChild(Point point) const { return *this; }
	virtual bool HitTest(Point point) const { return true; }
	virtual bool Handler(Msg msg, Para para) { return true; }
};


class DesktopObject : protected WndObject {
protected:
	DesktopObject(unique_ptr<IWndBase> desktop) : WndObject(std::move(desktop)) {}
	~DesktopObject() {}
public:
	WNDDESIGNCORE_API static DesktopObject& Get();

	virtual void OnChildRegionChange(WndObject& child) override pure;
	virtual void AddChild(WndObject& child) pure;
	virtual void MessageLoop() pure;
	virtual void Terminate() pure;
};


END_NAMESPACE(WndDesign)