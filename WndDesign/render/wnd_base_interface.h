#pragma once

#include "../message/message.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class ObjectBase;


struct IWndBase {
	WNDDESIGN_API static unique_ptr<IWndBase> Create(ObjectBase& object);

	virtual ~IWndBase();


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
	virtual void AddChild(IWndBase& child_wnd) pure;
	virtual void RemoveChild(IWndBase& child_wnd) pure;


	///////////////////////////////////////////////////////////
	////                     Composite                     ////
	///////////////////////////////////////////////////////////
	virtual void RegionUpdated() pure;
	virtual void CompositeStyleUpdated() pure;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
	virtual void SetCapture() pure;
	virtual void ReleaseCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseFocus() pure;
};


END_NAMESPACE(WndDesign)