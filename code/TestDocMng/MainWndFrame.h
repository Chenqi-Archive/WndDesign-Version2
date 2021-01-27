#pragma once

#include "WndDesign.h"


class MainWnd;

class MainWndFrame : public WndFrame {
private:
	unique_ptr<MainWnd> main_wnd;
public:
	MainWndFrame();
	~MainWndFrame();
};