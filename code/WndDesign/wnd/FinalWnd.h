#pragma once

#include "Wnd.h"


BEGIN_NAMESPACE(WndDesign)


// Final window base class that does not contain any child window, inherited by TextBox and ImageBox.
class FinalWnd : public Wnd {
private:
	WndObject::IsMyChild;
	WndObject::RegisterChild;
	WndObject::UnregisterChild;
	WndObject::RemoveChild;
	virtual void OnChildDetach(WndObject& child) override final {}
	WndObject::SetChildData;
	WndObject::GetChildData;
	WndObject::GetChildRegion;
	WndObject::SetChildRegion;
	virtual void OnChildRegionChange(WndObject& child) override final {}
	virtual const WndObject& HitTestChild(Point point) const override final { return *this; }
public:
	FinalWnd(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~FinalWnd() {}
};


END_NAMESPACE(WndDesign)