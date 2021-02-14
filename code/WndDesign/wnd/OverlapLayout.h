#pragma once

#include "Wnd.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;


class OverlapLayout : public Wnd {
public:
	using Style = Wnd::Style;

public:
	OverlapLayout(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~OverlapLayout() {}


	//// child windows ////
private:
	struct ChildWndContainer {
		WndObject& wnd;
		Rect region;
		//uint z_index;
		list<ChildWndContainer>::iterator list_index;
	};

	// frontmost(begin) ---> endmost(end)
	list<ChildWndContainer> _child_wnds;  // or use spatial index instead

private:
	static void SetChildData(WndObject& child, ChildWndContainer& child_container) {
		WndObject::SetChildData<ChildWndContainer*>(child, &child_container);
	}
	static ChildWndContainer& GetChildData(WndObject& child) {
		return *WndObject::GetChildData<ChildWndContainer*>(child);
	}

public:
	void AddChild(WndObject& child);
	virtual void OnChildDetach(WndObject& child) override;


	//// layout update ////
private:
	virtual void OnChildRegionUpdate(WndObject& child) override;
	virtual const Rect UpdateContentLayout(Size client_size) override;


	//// painting and composition ////
private:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const override;


	//// message handling ////
protected:
	virtual const HitTestInfo ClientHitTest(Size client_size, Point point) const override;
};


END_NAMESPACE(WndDesign)