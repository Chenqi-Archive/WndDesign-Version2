#pragma once

#include "timer.h"
#include "../wnd/Wnd.h"


BEGIN_NAMESPACE(WndDesign)


class FadeAnimation {
public:
	FadeAnimation(Wnd& wnd, std::function<void(void)> callback) :
		wnd(static_cast<WndHelper&>(wnd)), timer([&]() { OnTimer(); }),
		delta(0), target_opacity(0), callback(callback) {
	}
	~FadeAnimation() {}

private:
	static constexpr uint period = 40;  // 40ms
	struct WndHelper : public Wnd { public: Wnd::GetStyle; Wnd::CompositeEffectChanged; };

private:
	WndHelper& wnd;
	Timer timer;
	short delta;
	uchar target_opacity;

public:
	std::function<void(void)> callback;

private:
	static short abs(short a) { return a > 0 ? a : -a; }
	static short div_ceil(short a, short b) { return (a + b - 1) / b; }
	static uint div_ceil(uint a, uint b) { return (a + b - 1) / b; }

private:
	void OnTimer() {
		uchar& current_opacity = wnd.GetStyle().composite._opacity;
		if (current_opacity == target_opacity) {
			timer.Stop(); callback(); return;
		}
		assert(delta != 0);
		current_opacity = abs((short)target_opacity - (short)current_opacity) > abs(delta) ?
			(uchar)((short)current_opacity + delta) : target_opacity;
		wnd.CompositeEffectChanged();
	}

public:
	void Set(uint time_to_finish, uchar target_opacity = 0) {
		Stop();
		uchar& current_opacity = wnd.GetStyle().composite._opacity;
		if (time_to_finish == 0) {
			if (target_opacity != current_opacity) {
				current_opacity = target_opacity; 
				wnd.CompositeEffectChanged();
			}
			callback(); return;
		} else {
			if (target_opacity != current_opacity) {
				uint period_count = div_ceil(time_to_finish, period);
				delta = div_ceil((short)target_opacity - (short)current_opacity, (short)period_count);
				this->target_opacity = target_opacity;
				timer.Set(period);
			} else {
				timer.Set(time_to_finish);
			}
		}
	}
	void Stop() {
		timer.Stop();
	}
};


END_NAMESPACE(WndDesign)