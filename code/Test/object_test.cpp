#include "../WndDesign/WndDesign.h"
#include "../WndDesign/system/win32_aero_snap.h"

using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesign.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesign.lib")
#endif // _DEBUG


class MainWnd : public WndObject , public WndStyle {
public:
	MainWnd() {
		size.normal(800px, 500px).max(100pct, 100pct);
		position.setHorizontalCenter().setVerticalCenter();
		border.width(5.0).color(ColorSet::DarkGreen);
		background.setColor(ColorSet::LightGray);
		SetBackground(*background._background_resource);
	}
	virtual const Rect CalculateRegionOnParent(Size parent_size) override { 
		return CalculateRectFromStyle(*this, parent_size); 
	}
	virtual void OnSizeChange(Rect accessible_region) override {
		// change size or position style according to accessible region.
		// also when region on parent changed.
	}
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) override {
		return CalculateMinMaxSizeFromStyle(*this, parent_size); 
	}
	virtual const wstring GetTitle() const override { 
		return L"MainWnd"; 
	}
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const override {
		Point center = accessible_region.Center();
		figure_queue.Append(center - Vector(50, 50), new Rectangle(Size(100, 100), ColorSet::Firebrick));
		figure_queue.Append(center, new Ellipse(50, 50, 5.0, ColorSet::Moccasin, ColorSet::LemonChiffon));
		figure_queue.Append(point_zero, new Rectangle(accessible_region.size, border._width, border._color));
	}
	virtual bool Handler(Msg msg, Para para) override {
		if (msg == Msg::LeftDown) {
			AeroSnapDraggingEffect(*this, GetMouseMsg(para).point);
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