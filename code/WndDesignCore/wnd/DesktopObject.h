#pragma once

#include "WndObject.h"

#include <functional>


BEGIN_NAMESPACE(WndDesign)


class ABSTRACT_BASE DesktopObject : protected WndObject {
protected:
	DesktopObject(unique_ptr<IWndBase> desktop) : WndObject(std::move(desktop)) {}
	~DesktopObject() {}

public:
	WNDDESIGNCORE_API static DesktopObject& Get();

public:
	using HANDLE = void*;
	virtual void AddChild(WndObject& child, std::function<void(HANDLE hWnd)> callback = nullptr) pure;
	void RemoveChild(WndObject& child) { if (IsMyChild(child)) { WndObject::RemoveChild(child); } }

public:
	virtual void CommitReflowQueue() pure;
	virtual void CommitRedrawQueue() pure;

public:
	virtual void MessageLoop() pure;
	virtual void Terminate() pure;
};

extern DesktopObject& desktop;  // initialized in DesktopObject.cpp (both in dll and in lib)


END_NAMESPACE(WndDesign)