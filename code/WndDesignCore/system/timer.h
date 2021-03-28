#pragma once

#include "../common/uncopyable.h"

#include <functional>


BEGIN_NAMESPACE(WndDesign)


class Timer : public Uncopyable {
private:
	using HANDLE = void*;
	HANDLE timer;
public:
	Timer(std::function<void(void)> callback) : timer(nullptr), callback(callback) {}
	~Timer() { Stop(); }
public:
	bool IsSet() const { return timer != nullptr; }
	WNDDESIGNCORE_API void Set(uint period);
	WNDDESIGNCORE_API void Stop();
public:
	std::function<void(void)> callback;
};


END_NAMESPACE(WndDesign)