#pragma once

#include "Wnd.h"


BEGIN_NAMESPACE(WndDesign)


// Final window base class that does not contain any child window, inherited by TextBox and ImageBox.
class FinalWnd : public Wnd {
public:
	FinalWnd(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~FinalWnd() {}
private:
	WndObject::IsMyChild;
	WndObject::RegisterChild;
	WndObject::UnregisterChild;
	WndObject::RemoveChild;
	virtual void OnChildDetach(WndObject& child) override final {}
	WndObject::SetChildData;
	WndObject::GetChildData;
	WndObject::UpdateChildRegion;
	WndObject::SetChildRegionStyle;
	WndObject::SetChildRegion;
	WndObject::GetChildRegion;
	virtual void ChildRegionMayChange(WndObject& child) override final {}
	virtual void OnChildRegionUpdate(WndObject& child) override final {}
	virtual void OnChildTitleChange(WndObject& child) override final {}
	WndObject::CompositeChild;
};


END_NAMESPACE(WndDesign)