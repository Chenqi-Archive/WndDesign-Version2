#pragma once

#include "../wnd/TextBox.h"

#include <functional>


BEGIN_NAMESPACE(WndDesign)


class MessageBox : protected TextBox {
private:
	MessageBox();
	~MessageBox();
public:
	static MessageBox& Get();

private:
	std::function<void(void)> callback;

private:
	void Hide();
public:
	void Alert(const wchar message[], std::function<void(void)> callback = nullptr);

private:
	virtual void NonClientHandler(Msg msg, Para para) override;
};

inline MessageBox& GetMessageBox() { return MessageBox::Get(); }


END_NAMESPACE(WndDesign)