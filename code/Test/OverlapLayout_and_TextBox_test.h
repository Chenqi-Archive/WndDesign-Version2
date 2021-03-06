#include "../WndDesign/WndDesign.h"
#include "../WndDesign/wnd/OverlapLayout.h"
#include "../WndDesign/wnd/TextBox.h"


using namespace WndDesign;


class MainWnd : public OverlapLayout {
private:
	struct Style : OverlapLayout::Style {
		Style() {
			width.normal(500px).max(100pct);
			height.normal(400px).max(100pct);
			position.setHorizontalCenter().setVerticalCenter();
			//composite.opacity(0x7F);
			border.width(5).color(ColorSet::DarkGreen).setResizer(CreateDefaultBorderResizer());
			background.setColor(ColorSet::Goldenrod);
		}
	};
public:
	MainWnd() : OverlapLayout(std::make_unique<Style>()) {
		//AllocateLayer();
	}
	~MainWnd() {}
	virtual const wstring GetTitle() const override { return L"OverlapLayout and TextBox test"; }
};


extern const wchar text[];

class MyTextBox : public TextBox {
private:
	struct Style : TextBox::Style {
		Style() {
			width.normal(length_auto).min(200px).max(100pct);
			height.normal(length_auto).min(200px).max(100pct);
			position.left(0px).top(0px);
			composite.opacity(0x7F);
			border.width(10).radius(20).color(ColorSet::BlueViolet).setResizer(CreateDefaultBorderResizer());
			background.setColor(ColorSet::LightGray);
			padding.setAll(50px);
		}
	};
public:
	MyTextBox() : TextBox(std::make_unique<Style>(), wstring(text)) { 
		//AllocateLayer();
	}
	~MyTextBox() {}
};


int main() {
	MainWnd main_wnd;
	MyTextBox my_text_box;
	main_wnd.AddChild(my_text_box);
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}


const wchar text[] = L"\
To be, or not to be, that is the question:\
Whether 'tis nobler in the mind to suffer\
The slings and arrows of outrageous fortune,\
Or to take arms against a sea of troubles\
And by opposing end them.To die??to sleep,\
No more;and by a sleep to say we end\
The heart - ache and the thousand natural shocks\
That flesh is heir to : 'tis a consummation\
Devoutly to be wish'd. To die, to sleep;\
To sleep, perchance to dream??ay, there's the rub:\
For in that sleep of death what dreams may come,\
When we have shuffled off this mortal coil,\
Must give us pause??there's the respect\
That makes calamity of so long life.\
For who would bear the whips and scorns of time,\
Th'oppressor's wrong, the proud man's contumely,\
The pangs of dispriz'd love, the law's delay,\
The insolence of office, and the spurns\
That patient merit of th'unworthy takes,\
When he himself might his quietus make\
With a bare bodkin ? Who would fardels bear,\
To gruntand sweat under a weary life,\
But that the dread of something after death,\
The undiscovere'd country, from whose bourn\
No traveller returns, puzzles the will,\
And makes us rather bear those ills we have\
Than fly to others that we know not of ?\
Thus conscience does make cowards of us all,\
And thus the native hue of resolution\
Is sicklied o'er with the pale cast of thought,\
And enterprises of great pitch and moment\
With this regard their currents turn awry\
And lose the name of action.\
";