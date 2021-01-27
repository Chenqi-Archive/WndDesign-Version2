#include "MainWndFrame.h"
#include "MainWnd.h"


MainWndFrame::MainWndFrame() {
	main_wnd = std::make_unique<MainWnd>();
}

MainWndFrame::~MainWndFrame() {
}
