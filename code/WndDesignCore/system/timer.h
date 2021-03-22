#pragma once

#include "../common/uncopyable.h"


BEGIN_NAMESPACE(WndDesign)

using HANDLE = void*;


class Timer : public Uncopyable {
private:
	HANDLE timer;
public:
	Timer() : timer(nullptr) {}
	~Timer() { Stop(); }
public:
	bool IsSet() const { return timer != nullptr; }
	WNDDESIGNCORE_API void Set(uint period);
	WNDDESIGNCORE_API void Stop();
	virtual void OnAlert() {}
};


END_NAMESPACE(WndDesign)