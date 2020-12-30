#pragma once

#include "wnd_base_interface.h"
#include "composite_style.h"
#include "../common/core.h"
#include "../common/uncopyable.h"
#include "../geometry/geometry.h"
#include "../message/msg_base.h"


BEGIN_NAMESPACE(WndDesign)


class ObjectBase : public Uncopyable{
private:
	unique_ptr<IWndBase> wnd;

protected:
	ObjectBase(nullptr_t) : wnd(nullptr) {}
	ObjectBase() : wnd(IWndBase::Create(*this)) {}
	~ObjectBase() {}

public:
	/* Callback functions (called by WndBase) */
	virtual const Rect CalculatePosition(Size parent_size) const { return Rect(point_zero, parent_size); };
	virtual const CompositeStyle GetCompositeStyle() const { return CompositeStyle(); }  // No need.
	virtual bool Handler(Msg msg, Para para) { return true; }
};


inline static const class _NullWnd : public ObjectBase { _NullWnd() :ObjectBase(nullptr) {} } nullwnd;


END_NAMESPACE(WndDesign)