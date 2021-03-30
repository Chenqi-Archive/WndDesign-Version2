#include "MessageBox.h"
#include "../system/win32.h"


BEGIN_NAMESPACE(WndDesign)


MessageBox& MessageBox::Get() {
	static MessageBox message_box;
	return message_box;
}


struct MessageBoxStyle : public TextBox::Style {
	MessageBoxStyle() {
		width.setFixed(300px);
		height.max(250px);
		position.setHorizontalCenter().setVerticalCenter();
		padding.setAll(30px);
		background.setColor(ColorSet::MediumPurple);

		paragraph.text_align(TextAlign::Center);
		font.size(20);
	}
};

static constexpr uint expire_time = 5000;


MessageBox::MessageBox() :
	TextBox(std::make_unique<MessageBoxStyle>(), L""), timer([&]() { Hide(); }) {
	desktop.AddChild(*this);
	hwnd = GetWndHandle(*this);
	HideWndFromTaskbar(hwnd); Hide();
}

MessageBox::~MessageBox() {}

void MessageBox::Hide() {
	timer.Stop();
	HideWnd(hwnd);
}

void MessageBox::NonClientHandler(Msg msg, Para para) {
	if (msg == Msg::LoseFocus) { Hide(); }
	if (msg == Msg::KeyDown && GetKeyMsg(para).key == Key::Escape) { Hide(); }
}

void MessageBox::ShowMessage(const wchar message[]) {
	SetText(message);
	ShowWnd(hwnd); SetFocus();
	timer.Set(expire_time);
}


END_NAMESPACE(WndDesign)