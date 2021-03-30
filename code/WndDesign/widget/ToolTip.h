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
	using HANDLE = void*;
	HANDLE hwnd = nullptr;
	Timer timer;
	FadeAnimation fade_animation;

private:
	void FadeInBegin();
	void FadeInEnd();
	void FadeOutBegin();
	void Hide();

private:
	virtual void Handler(Msg msg, Para para) override;

public:
	void OnMouseEnter(const wchar text[]);
	void OnMouseLeave() { Hide(); }
};

inline ToolTip& GetToolTip() { return ToolTip::Get(); }


END_NAMESPACE(WndDesign)