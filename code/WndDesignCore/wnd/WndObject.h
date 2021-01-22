#pragma once

#include "wnd_base_interface.h"
#include "../common/uncopyable.h"
#include "../message/msg_base.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::pair;
using std::wstring;


class WndObject : public Uncopyable {
private:
	friend class WndBase;
	unique_ptr<IWndBase> wnd;

private:
	friend class DesktopObject;
	WndObject(unique_ptr<IWndBase> desktop): wnd(std::move(desktop)) {}
protected:
	WndObject() : wnd(IWndBase::Create(*this)) {}
	~WndObject() {}


	//// child and parent window relation ////
public:
	bool HasParent() const { return wnd->HasParent(); }
	WndObject& GetParent() const { return wnd->GetParent(); }
protected:
	void RegisterChild(WndObject& child) { wnd->AddChild(*child.wnd); }
	void UnregisterChild(WndObject& child) { wnd->RemoveChild(*child.wnd); }
public:
	virtual void OnChildDetach(WndObject& child) {}

	//// child window data ////
protected:
	using Data = unsigned long long; 
	static_assert(sizeof(Data) == 8);
private:
	Data data = 0;
protected:
	// data is used for parent to store information at child windows.
	static void SetChildData(WndObject& child, Data data) {child.data = data;}
	static Data GetChildData(WndObject& child) { return child.data; }


	//// window region ////
public:
	const Rect GetAccessibleRegion() const { return wnd->GetAccessibleRegion(); }
	const Size GetSize() const { return GetAccessibleRegion().size; }
	const Rect GetDisplayRegion() const { return wnd->GetDisplayRegion(); }
	const Vector SetDisplayOffset(Point display_offset) { return wnd->SetDisplayOffset(display_offset); }
	const Point GetDisplayOffset() const { return GetDisplayRegion().point; }
	const Vector ScrollView(Vector vector) { return SetDisplayOffset(GetDisplayOffset() + vector); }  // returns the real vector shifted
	const Rect GetRegionOnParnet() const { return wnd->GetRegionOnParent(); }
protected:
	static void SetChildRegion(WndObject& child, Rect region_on_parent) { child.wnd->SetRegionOnParent(region_on_parent); }
private:
	void SetAccessibleRegion(Rect accessible_region) { wnd->SetAccessibleRegion(accessible_region); }
private:
	virtual void OnSizeChange(Rect accessible_region) {}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) {}  // for scrolling
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) {}  // for lazy-loading

	// region computing
protected:
	void RegionChanged() { if (HasParent()) { GetParent().OnChildRegionChange(*this); } }
public:
	/* called by parent window to get the child window's region */
	const Rect CalculateRegion(Size parent_size) {
		Rect region_on_parent = CalculateRegionOnParent(parent_size);
		SetAccessibleRegion(CalculateAccessibleRegion(parent_size, region_on_parent.size));
		return region_on_parent;
	}
private:
	virtual void OnChildRegionChange(WndObject& child) {}
	virtual const Rect CalculateRegionOnParent(Size parent_size) { return region_empty; }
	virtual const Rect CalculateAccessibleRegion(Size parent_size, Size display_region_size) { return Rect(point_zero, display_region_size); }


	//// other window styles ////
public:
	// used for desktop window.
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { return { size_min, size_max }; }
	virtual const wstring GetTitle() const { return wstring(); }


	//// painting and composition ////
public:
	void SetBackground(const Background& background) { wnd->SetBackground(background); }
	void AllocateLayer() { wnd->AllocateLayer(); }
	void Invalidate(Rect region) { wnd->Invalidate(region); }
protected:
	/* called by parent window when parent window is painting */
	void Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const { wnd->Composite(figure_queue, parent_invalid_region); }
private:
	/* draw anything to the invalid region except for background */
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}


	//// message handling ////
public:
	void SetCapture() { wnd->SetCapture(); }
	void SetFocus() { wnd->SetFocus(); }
	void ReleaseCapture() { wnd->ReleaseCapture(); }
	void ReleaseFocus() { wnd->ReleaseFocus(); }
private:
	virtual bool Handler(Msg msg, Para para) { return true; }
	virtual const WndObject& HitTestChild(Point point) const { return *this; }
	virtual bool HitTest(Point point) const { return true; }
};


class DesktopObject : protected WndObject {
protected:
	DesktopObject(unique_ptr<IWndBase> desktop) : WndObject(std::move(desktop)) {}
	~DesktopObject() {}
public:
	WNDDESIGNCORE_API static DesktopObject& Get();
	virtual void AddChild(WndObject& child) pure;
	virtual void RemoveChild(WndObject& child) pure;
	virtual void OnChildRegionChange(WndObject& child) override pure;
	virtual void MessageLoop() pure;
	virtual void Terminate() pure;
};

inline DesktopObject& Desktop() { return DesktopObject::Get(); }


END_NAMESPACE(WndDesign)