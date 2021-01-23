#include "../WndDesign/WndDesign.h"

using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesign.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesign.lib")
#endif // _DEBUG


class MainWnd : public WndObject , public WndStyle {
public:
	MainWnd() {
		size.setFixed(800px, 500px);
		position.setHorizontalCenter().setVerticalCenter();
		border.width(5.0).color(ColorSet::DarkGreen);
		background.setColor(ColorSet::LightGray);
		SetBackground(*background._background_resource);
	}
	virtual const Rect CalculateRegionOnParent(Size parent_size) { 
		return CalculateRectFromStyle(*this, parent_size); 
	}
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { 
		return CalculateMinMaxSizeFromStyle(*this, parent_size); 
	}
	virtual const wstring GetTitle() const {
		return L"MainWnd"; 
	}
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {
		Point center = accessible_region.Center();
		figure_queue.Append(center - Vector(50, 50), new Rectangle(Size(100, 100), ColorSet::Firebrick));
		figure_queue.Append(center, new Ellipse(50, 50, 5.0, ColorSet::Moccasin, ColorSet::LemonChiffon));
		figure_queue.Append(point_zero, new Rectangle(accessible_region.size, border._width, border._color));
	}
	virtual bool Handler(Msg msg, Para para) { 
		return true; 
	}
};


int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}