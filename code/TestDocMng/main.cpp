#include "MainWndFrame.h"


int main() {
	MainWndFrame main_wnd_frame;
	desktop.AddChild(main_wnd_frame);
	desktop.MessageLoop();
	return 0;
}