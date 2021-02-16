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
	void Set(uint period);
	void Stop();
	virtual void OnAlert() {}
};


END_NAMESPACE(WndDesign)