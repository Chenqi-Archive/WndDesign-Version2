#pragma once

#include "../wnd/TextBox.h"
#include "../message/timer.h"


BEGIN_NAMESPACE(WndDesign)


class MessageBox : protected TextBox {
private:
	MessageBox();
	~MessageBox();
public:
	static MessageBox& Get();

private:
	using HANDLE = void*;
	HANDLE hwnd;
	Timer timer;

private:
	void Hide();

private:
	virtual void NonClientHandler(Msg msg, Para para) override;

public:
	void ShowMessage(const wchar message[]);
};

inline MessageBox& GetMessageBox() { return MessageBox::Get(); }

inline void ShowMessageBox(const wchar message[]) { GetMessageBox().ShowMessage(message); }


END_NAMESPACE(WndDesign)