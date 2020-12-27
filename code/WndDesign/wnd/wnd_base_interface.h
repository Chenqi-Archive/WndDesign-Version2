#pragma once

#include "../common/common.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;

class ObjectBase;


struct IWndBase {
	WNDDESIGNCORE_API static unique_ptr<IWndBase> Create(ObjectBase& object);

	virtual ~IWndBase() pure {}


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
	virtual void AddChild(IWndBase& child_wnd) pure;
	virtual void RemoveChild(IWndBase& child_wnd) pure;


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
	virtual void PositionUpdated() pure;
	virtual void CompositeStyleUpdated() pure;


	///////////////////////////////////////////////////////////
	////                       Layer                       ////
	///////////////////////////////////////////////////////////




	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
	virtual void SetCapture() pure;
	virtual void ReleaseCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseFocus() pure;
};


END_NAMESPACE(WndDesign)