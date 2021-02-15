#include "scrollbar.h"
#include "../wnd/Wnd.h"
#include "../figure/figure_queue.h"
#include "../figure/figure_types.h"


BEGIN_NAMESPACE(WndDesign)


void Scrollbar::SetWndCapture(Wnd& wnd) { wnd.SetScrollbarCapture(); }
void Scrollbar::ReleaseWndCapture(Wnd& wnd) { wnd.ReleaseCapture(); }


class DefaultScrollbar : public Scrollbar {
private:
	static inline const uint width = 20;
	static inline const Color background_color = ColorSet::Gray;
	static inline const Color slider_color = ColorSet::DarkGray;
private:
	uint _entire_length;
	uint _display_length;
	uint _display_offset;
public:
	DefaultScrollbar() :_entire_length(0), _display_length(0), _display_offset(0) {}
	virtual ~DefaultScrollbar() override {}
	virtual bool IsAuto() const override { return true; }
	virtual bool HasMargin() const override { return _display_length < _entire_length; }
	virtual const Margin GetMargin() const override { return Margin{ 0, 0, HasMargin() ? width : 0, 0 }; }
	virtual void Update(Size entire_size, Rect display_region) override {
		assert(Rect(point_zero, entire_size).Contains(display_region));
		_entire_length = entire_size.height;
		_display_length = display_region.size.height;
		_display_offset = (uint)display_region.point.x;
	}

	virtual bool HitTest(Size display_size, Point point) override {
		if (!HasMargin()) { return false; }
		return Rect((int)(display_size.width - width), 0, width, display_size.height).Contains(point);
	}
	virtual bool Handler(Wnd& wnd, Msg msg, Para para) override {
		return true;
	}
	virtual bool IsVisible() const { return HasMargin(); }
	virtual void OnPaint(FigureQueue& figure_queue, Size display_size) override {
		if (_entire_length == 0) { return; }
		uint slider_length = display_size.height * _display_length / _entire_length;
		uint slider_offset = display_size.height * _display_offset / _entire_length;
		figure_queue.Append(Point((int)(display_size.width - width), 0), new Rectangle(Size(width, display_size.height), background_color));
		figure_queue.Append(Point((int)(display_size.width - width), 0), new Rectangle(Size(width, display_size.height), slider_color));
	}
};

unique_ptr<Scrollbar> CreateDefaultScrollbar() {
	return std::make_unique<DefaultScrollbar>();
}


END_NAMESPACE(WndDesign)