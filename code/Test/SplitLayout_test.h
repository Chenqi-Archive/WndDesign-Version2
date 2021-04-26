#include "../WndDesign/WndDesign.h"
#include "../WndDesign/wnd/SplitLayout.h"
#include "../WndDesign/wnd/EditBox.h"


using namespace WndDesign;


class MainWnd : public SplitLayout {
private:
	struct Style : SplitLayout::Style {
		Style() {
			width.normal(70pct).max(100pct);
			height.normal(80pct).max(100pct);
			position.setHorizontalCenter().setVerticalCenter();
			border.width(5).color(ColorSet::DarkGreen).setResizer(CreateAeroSnapBorderResizer());
			background.setColor(ColorSet::LightGray);
			split_line.position(30pct).width(5).color(ColorSet::DarkMagenta);
		}
	};
public:
	MainWnd() : SplitLayout(std::make_unique<Style>()) {
		//AllocateLayer(); 
	}
};


class TextArea : public EditBox {
private:
	struct Style : EditBox::Style {
		Style() {
			width.setFixed(100pct);
			height.setFixed(100pct);
			border.width(3).color(ColorSet::Honeydew);
			padding.set(20px, 10px, 20px, 10px);
			background.setColor(ColorSet::YellowGreen);
			font.size(20);
		}
	};
public:
	TextArea() : EditBox(std::make_unique<Style>(), L"Type something here...") {
		//AllocateLayer();
	}
};


int main() {
	MainWnd main_wnd;
	TextArea text_area1, text_area2;
	main_wnd.SetChildLeft(text_area1);
	main_wnd.SetChildRight(text_area2);
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}