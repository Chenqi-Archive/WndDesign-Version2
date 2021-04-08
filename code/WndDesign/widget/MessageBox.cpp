#include "MessageBox.h"
#include "../wnd/DesktopObject.h"
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
		composite.z_index(127);
		padding.setAll(30px);
		background.setColor(ColorSet::DarkGray);
		paragraph.text_align(TextAlign::Center);
		font.size(20);
	}
};

static constexpr uint expire_time = 5000;


MessageBox::MessageBox() : TextBox(std::make_unique<MessageBoxStyle>(), L"") {}

MessageBox::~MessageBox() {}

void MessageBox::Hide() {
	desktop.RemoveChild(*this);
	if (callback) { callback(); callback = nullptr; }
}

void MessageBox::Alert(const wchar message[], std::function<void(void)> callback) {
	SetText(message); this->callback = callback;
	desktop.AddChild(*this, [](HANDLE hWnd) { HideWndFromTaskbar(hWnd); });
}

void MessageBox::NonClientHandler(Msg msg, Para para) {
	if (msg == Msg::LeftDown) { Hide(); }
	if (msg == Msg::KeyDown && GetKeyMsg(para).key == Key::Escape) { Hide(); }
}


END_NAMESPACE(WndDesign)