#include "border_resizer.h"
#include "border_helper.h"
#include "../system/win32_aero_snap.h"
#include "../wnd/Wnd.h"


BEGIN_NAMESPACE(WndDesign)


void BorderResizer::Handler(Wnd& wnd, Rect window_region, uint border_width, Msg msg, Para para) {
	if (msg == Msg::MouseEnter) { SetCursor(Cursor::Default); } 
}
void BorderResizer::ResizeWnd(Wnd& wnd, Rect old_window_region, Margin margin_to_extend) {
	wnd.ResetRegionOnParent(old_window_region, margin_to_extend);
}
void BorderResizer::SetWndCapture(Wnd& wnd) { wnd.SetBorderCapture(); }
void BorderResizer::ReleaseWndCapture(Wnd& wnd) { wnd.ReleaseCapture(); }


inline void SetCursorDependingOnBorderPosition(BorderPosition border_position) {
	switch (border_position) {
	case BorderPosition::Left: SetCursor(Cursor::Resize0); break;
	case BorderPosition::Top: SetCursor(Cursor::Resize90); break;
	case BorderPosition::Right: SetCursor(Cursor::Resize0); break;
	case BorderPosition::Bottom: SetCursor(Cursor::Resize90); break;
	case BorderPosition::LeftTop: SetCursor(Cursor::Resize135); break;
	case BorderPosition::RightTop: SetCursor(Cursor::Resize45); break;
	case BorderPosition::LeftBottom: SetCursor(Cursor::Resize45); break;
	case BorderPosition::RightBottom: SetCursor(Cursor::Resize135); break;
	}
}


class DefaultBorderResizer : public BorderResizer {
public:
	DefaultBorderResizer() {}
	~DefaultBorderResizer() {}

private:
	bool _is_mouse_down = false;
	Point _mouse_down_position = point_zero;
	BorderPosition _mouse_down_border_position = BorderPosition::None;
	Rect _mouse_down_window_region = region_empty;
public:
	virtual void Handler(Wnd& wnd, Rect window_region, uint border_width, Msg msg, Para para) override {
		if (!IsMouseMsg(msg)) { return; }
		MouseMsg& mouse_msg = GetMouseMsg(para);
		BorderPosition border_position;
		if (!_is_mouse_down) { border_position = HitTestBorderPosition(window_region.size, border_width, mouse_msg.point); }
		switch (msg) {
		case Msg::MouseMove:
			if (!_is_mouse_down) {
				SetCursorDependingOnBorderPosition(border_position);
			} else {
				Point current_mouse_position = mouse_msg.point;
				Margin margin_to_extend = {};
				switch (_mouse_down_border_position) {
				case BorderPosition::Left: margin_to_extend.left += _mouse_down_position.x - current_mouse_position.x; break;
				case BorderPosition::Top: margin_to_extend.top += _mouse_down_position.y - current_mouse_position.y; break;
				case BorderPosition::Right: margin_to_extend.right += current_mouse_position.x - _mouse_down_position.x; break;
				case BorderPosition::Bottom: margin_to_extend.bottom += current_mouse_position.y - _mouse_down_position.y; break;
				case BorderPosition::LeftTop:
					margin_to_extend.left += _mouse_down_position.x - current_mouse_position.x;
					margin_to_extend.top += _mouse_down_position.y - current_mouse_position.y;
					break;
				case BorderPosition::RightTop:
					margin_to_extend.right += current_mouse_position.x - _mouse_down_position.x;
					margin_to_extend.top += _mouse_down_position.y - current_mouse_position.y;
					break;
				case BorderPosition::LeftBottom:
					margin_to_extend.left += _mouse_down_position.x - current_mouse_position.x;
					margin_to_extend.bottom += current_mouse_position.y - _mouse_down_position.y;
					break;
				case BorderPosition::RightBottom:
					margin_to_extend.right += current_mouse_position.x - _mouse_down_position.x;
					margin_to_extend.bottom += current_mouse_position.y - _mouse_down_position.y;
					break;
				}
				if (!margin_to_extend.IsEmpty()) {
					ResizeWnd(wnd, _mouse_down_window_region, margin_to_extend);
				}
			}
			break;
		case Msg::LeftDown:
			assert(!_is_mouse_down);
			assert(border_position != BorderPosition::None);
			_is_mouse_down = true;
			_mouse_down_position = mouse_msg.point;
			_mouse_down_border_position = border_position;
			_mouse_down_window_region = window_region;
			SetWndCapture(wnd);
			break;
		case Msg::LeftUp:
			if (_is_mouse_down) {
				_is_mouse_down = false;
				ReleaseWndCapture(wnd);
			}
			break;
		}
	}
};


class AeroSnapBorderResizer : public BorderResizer {
public:
	AeroSnapBorderResizer() {}
	~AeroSnapBorderResizer() {}
private:
	virtual void Handler(Wnd& wnd, Rect window_region, uint border_width, Msg msg, Para para) override {
		if (IsMouseMsg(msg)) {
			MouseMsg& mouse_msg = GetMouseMsg(para);
			BorderPosition border_position = HitTestBorderPosition(window_region.size, border_width, mouse_msg.point);
			SetCursorDependingOnBorderPosition(border_position);
			if (msg == Msg::LeftDown) {
				AeroSnapBorderResizingEffect(wnd, mouse_msg.point, border_position);
			}
		}
	}
};

unique_ptr<BorderResizer> CreateEmptyBorderResizer() { return std::make_unique<BorderResizer>(); }
unique_ptr<BorderResizer> CreateDefaultBorderResizer() { return std::make_unique<DefaultBorderResizer>(); }
unique_ptr<BorderResizer> CreateAeroSnapBorderResizer() { return std::make_unique<AeroSnapBorderResizer>(); }


END_NAMESPACE(WndDesign)