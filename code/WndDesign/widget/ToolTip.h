#pragma once

#include "../wnd/TextBox.h"
#include "../message/timer.h"
#include "../message/fade_animation.h"


BEGIN_NAMESPACE(WndDesign)


class ToolTip : protected TextBox {
private:
	ToolTip();
	~ToolTip();
public:
	static ToolTip& Get();

private:
	Timer timer;
	FadeAnimation fade_animation;

private:
	void FadeInBegin();
	void FadeInEnd();
	void FadeOutBegin();
	void Hide();

private:
	void OnMouseEnter(const wchar text[]);

public:
	void Track(Msg msg, const wchar text[]) {
		if (msg == Msg::MouseEnter) { OnMouseEnter(text); return; }
		if (msg != Msg::MouseMove) { Hide(); return; }
	}
};

inline ToolTip& GetToolTip() { return ToolTip::Get(); }


END_NAMESPACE(WndDesign)