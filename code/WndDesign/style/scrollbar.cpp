#include "scrollbar.h"
#include "../wnd/Wnd.h"
#include "../figure/figure_queue.h"
#include "../figure/figure_types.h"


BEGIN_NAMESPACE(WndDesign)


void Scrollbar::SetWndCapture(Wnd& wnd) { wnd.SetScrollbarCapture(); }
void Scrollbar::ReleaseWndCapture(Wnd& wnd) { wnd.ReleaseCapture(); }
void Scrollbar::InvalidateWnd(Wnd& wnd, Rect region) { wnd.NonClientInvalidate(region + (_region.point - point_zero)); }
void Scrollbar::SendWndClientMessage(Wnd& wnd, Msg msg, Para para) {
	if (IsMouseMsg(msg)) { GetMouseMsg(para).point += (_region.point - point_zero) + wnd.GetDisplayOffset() - wnd.GetClientOffset(); }
	wnd.Handler(msg, para); 
}
void Scrollbar::SetWndScrollOffset(Wnd& wnd, Vector offset) { wnd.SetDisplayOffset(offset); }


class DefaultScrollbar : public Scrollbar {
public:
	DefaultScrollbar() {}
	virtual ~DefaultScrollbar() override {}

	// scroll offset update
private:
	uint _entire_length = 0;
	uint _display_length = 0;
	Vector _display_offset = vector_zero;
public:
	virtual void Update(Size entire_size, Rect display_region) override {
		//assert(Rect(point_zero, entire_size).Contains(display_region));
		_entire_length = entire_size.height;
		_display_length = display_region.size.height;
		_display_offset = display_region.point - point_zero;
	}

	// margin calculation
private:
	static inline const uint width = 20;
public:
	virtual bool IsMarginAuto() const override { return true; }
	virtual bool HasMargin() const override { return _display_length < _entire_length; }
	virtual const Margin GetMargin() const override { return Margin{ 0, 0, HasMargin() ? width : 0, 0 }; }

	// painting
private:
	static inline const Color frame_color = ColorSet::Silver;
	static inline const Color slider_color_normal = ColorSet::DarkGray;
	static inline const Color slider_color_hover = ColorSet::Gray;
	static inline const Color slider_color_down = ColorSet::DimGray;
private:
	Rect _frame_region = region_empty;
	Rect _slider_region = region_empty;
private:
	virtual void SizeUpdated(Wnd& wnd) override {
		Size display_size = GetDisplaySize();
		_frame_region = Rect((int)(display_size.width - width), 0, width, display_size.height);
		if (_entire_length == 0) {
			_slider_region = region_empty;
		} else {
			uint slider_length = (uint)round((double)display_size.height * _display_length / _entire_length);
			int slider_offset = (int)round((double)display_size.height * _display_offset.y / _entire_length);
			_slider_region = Rect((int)(display_size.width - width), slider_offset, width, slider_length);
		}
		InvalidateWnd(wnd, _frame_region);
	}
public:
	virtual bool IsVisible() const override { return HasMargin(); }
	virtual void OnPaint(FigureQueue& figure_queue) override {
		figure_queue.Append(_frame_region.point, new Rectangle(_frame_region.size, frame_color));
		figure_queue.Append(_slider_region.point, new Rectangle(_slider_region.size, _current_slider_color));
	}

	// hit-testing and message handling

	// slider message handling
private:
	enum class SliderState { Normal, Hover, Down } _slider_state = SliderState::Normal;
	Color _current_slider_color = slider_color_normal;
	int _mouse_down_position = 0;
	int _mouse_down_display_offset = 0;
private:
	void SetSliderColor(Wnd& wnd, Color slider_color) {
		if (_current_slider_color == slider_color) { return; }
		_current_slider_color = slider_color;
		InvalidateWnd(wnd, _slider_region);
	}
	bool IsCaptured() const { return _slider_state == SliderState::Down; }
public:
	void SliderHandler(Wnd& wnd, Msg msg, Para para) {
		switch (msg) {
		case Msg::MouseMove: 
			if (_slider_state == SliderState::Normal) { 
				_slider_state = SliderState::Hover; 
				SetSliderColor(wnd, slider_color_hover);
			} else if (_slider_state == SliderState::Down) {
				int current_mouse_position = GetMouseMsg(para).point.y;
				if (current_mouse_position != _mouse_down_position) {
					int target_offset = _mouse_down_display_offset +
						(int)round(((double)current_mouse_position - _mouse_down_position) * (int)_entire_length / GetDisplaySize().height);
					SetWndScrollOffset(wnd, Vector(_display_offset.x, target_offset));
				}
			}
			break;
		case Msg::MouseLeave: 
			_slider_state = SliderState::Normal;
			SetSliderColor(wnd, slider_color_normal);
			break;
		case Msg::LeftDown:
			_slider_state = SliderState::Down; 
			SetSliderColor(wnd, slider_color_down);
			SetWndCapture(wnd);
			_mouse_down_position = GetMouseMsg(para).point.y;
			_mouse_down_display_offset = _display_offset.y;
			break;
		case Msg::LeftUp: 
			_slider_state = SliderState::Hover;
			SetSliderColor(wnd, slider_color_hover);
			ReleaseWndCapture(wnd);
			break;
		}
	}

public:
	virtual bool HitTest(Point point) override {
		if (!HasMargin()) { return false; }
		return _frame_region.Contains(point);
	}
	virtual bool Handler(Wnd& wnd, Msg msg, Para para) override {
		switch (msg) {
		case Msg::MouseEnter: 
			SetCursor(Cursor::Default);
			break;
		case Msg::MouseWheel:
		case Msg::MouseWheelHorizontal: 
			SendWndClientMessage(wnd, msg, para); 
			break;
		case Msg::MouseMove:
		case Msg::LeftDown:
		case Msg::LeftUp:
			if (!IsCaptured() && !_slider_region.Contains(GetMouseMsg(para).point)) { break; }
			[[fallthrough]];
		case Msg::MouseLeave:
			SliderHandler(wnd, msg, para);
			break;
		}
		return true;
	}
};


unique_ptr<Scrollbar> CreateEmptyScrollbar() { return std::make_unique<Scrollbar>(); }
unique_ptr<Scrollbar> CreateDefaultScrollbar() { return std::make_unique<DefaultScrollbar>(); }


END_NAMESPACE(WndDesign)