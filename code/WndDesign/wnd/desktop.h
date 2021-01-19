#pragma once

#include "desktop_wnd.h"


BEGIN_NAMESPACE(WndDesign)

class DesktopLayer;


class Desktop : public Uncopyable {
private:
	Desktop() {}
	~Desktop() {}
public:
	static Desktop& Get();


private:
	list<DesktopWnd> windows;


public:
	int MessageLoop();

};

Desktop& GetDesktop() { return Desktop::Get(); }



class DesktopWndObject : public WndObject {

};




END_NAMESPACE(WndDesign)