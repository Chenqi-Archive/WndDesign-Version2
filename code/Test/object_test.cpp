#include "../WndDesign/WndDesign.h"
//#include "../WndDesign/wnd/OverlapView.h"

using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesign.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesign.lib")
#endif // _DEBUG


class MainWnd : public WndObject {
private:
	Point center = Point(400, 250);
public:
	MainWnd() {}
	virtual const Rect CalculateRegionOnParent(Size parent_size) { 
		return Rect(100, 100, 800, 500); 
	}
	virtual const wstring GetTitle() const { 
		return L"MainWnd"; 
	}
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
		figure_queue.Append(center - point_zero, new Ellipse(50, 50, 3.0, ColorSet::Moccasin, ColorSet::LemonChiffon));
	}
	virtual bool Handler(Msg msg, Para para) { 
		if (IsMouseMsg(msg)) {
			Point new_center = GetMouseMsg(para).point;
			if (new_center != center) {
				Invalidate(Rect(center - Vector(50, 50), Size(100, 100)));
				Invalidate(Rect(new_center - Vector(50, 50), Size(100, 100)));
				center = new_center;
			}
		}
		return true; 
	}
};


int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}