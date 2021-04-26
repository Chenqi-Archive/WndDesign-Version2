#include "SplitLayout.h"

#include <algorithm>


BEGIN_NAMESPACE(WndDesign)


void SplitLayout::SetChildLeft(WndObject& child) {
	RemoveChildLeft();
	RegisterChild(child);
	_child_left = &child;
	SetChildData(child, true);
}

void SplitLayout::SetChildRight(WndObject& child) {
	RemoveChildRight();
	RegisterChild(child);
	_child_right = &child;
	SetChildData(child, false);
}

void SplitLayout::RemoveChildLeft() {
	if (_child_left != nullptr) { WndObject::RemoveChild(*_child_left); }
}

void SplitLayout::RemoveChildRight() {
	if (_child_right != nullptr) { WndObject::RemoveChild(*_child_right); }
}

void SplitLayout::OnChildDetach(WndObject& child) {
	Wnd::OnChildDetach(child);
	bool is_left = GetChildData(child);
	is_left ? (_child_left = nullptr, Invalidate(_region_left)) : (_child_right = nullptr, Invalidate(_region_right));
}

void SplitLayout::UpdateChildRegion(WndObject& child, bool is_left) {
	Rect region = is_left ? _region_left : _region_right;
	Rect child_region = WndObject::UpdateChildRegion(child, region.size);
	SetChildRegion(child, region.Intersect(Rect(region.point, child_region.size)));
}

void SplitLayout::OnChildRegionUpdate(WndObject& child) {
	UpdateChildRegion(child, GetChildData(child));
}

const Rect SplitLayout::UpdateContentLayout(Size client_size) {
	if (_is_layout_invalid || client_size != GetClientSize()) {
		Style::SplitLineStyle& split_line = GetStyle().split_line;
		if (client_size.width <= split_line._width) {
			_region_left = _region_right = region_empty;
			_region_split_line = Rect(point_zero, client_size);
		} else {
			uint line_position_center = ValueTag(split_line._position).ConvertToPixel(client_size.width).AsUnsigned();
			uint line_position_left = line_position_center - split_line._width / 2;
			line_position_left = std::clamp(line_position_center, (uint)0, client_size.width - split_line._width);
			uint new_line_position_center = line_position_left + split_line._width / 2;
			if (new_line_position_center != line_position_center) {
				if (split_line._position.IsPixel()) {
					split_line._position = px(new_line_position_center);
				} else if (split_line._position.IsPercent()) {
					split_line._position = pct(new_line_position_center * 100 / client_size.width);
				}
			}
			uint line_position_right = line_position_left + split_line._width;
			_region_left = Rect(point_zero, Size(line_position_left, client_size.height));
			_region_right = Rect(Point(line_position_right, 0), Size(client_size.width - line_position_right, client_size.height));
			_region_split_line = Rect(Point(line_position_left, 0), Size(split_line._width, client_size.height));
		}
		if (_child_left != nullptr) { UpdateChildRegion(*_child_left, true); }
		if (_child_right != nullptr) { UpdateChildRegion(*_child_right, false); }
		Invalidate(region_infinite);
		_is_layout_invalid = false;
	}
	return Rect(point_zero, client_size);
}

void SplitLayout::OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {
	if (_child_left != nullptr) {
		Rect invalid_child_region = invalid_client_region.Intersect(_region_left);
		if (!invalid_child_region.IsEmpty()) {
			CompositeChild(*_child_left, figure_queue, invalid_child_region);
		}
	}
	if (_child_right != nullptr) {
		Rect invalid_child_region = invalid_client_region.Intersect(_region_right);
		if (!invalid_child_region.IsEmpty()) {
			CompositeChild(*_child_right, figure_queue, invalid_child_region);
		}
	}
	if (!_region_split_line.Intersect(invalid_client_region).IsEmpty()) {
		figure_queue.Append(_region_split_line.point, new Rectangle(_region_split_line.size, GetStyle().split_line._color));
	}
}

const Wnd::HitTestInfo SplitLayout::ClientHitTest(Size client_size, Point point) const {
	if (_region_left.Contains(point)) { return { _child_left, point - (_region_left.point - point_zero) }; }
	if (_region_right.Contains(point)) { return { _child_right, point - (_region_right.point - point_zero) }; }
	return {};
}

void SplitLayout::Handler(Msg msg, Para para) {
	switch (msg) {
	case Msg::LeftDown:
		if (_region_split_line.Contains(GetMouseMsg(para).point)) {
			SetCapture();
			_is_mouse_dragging = true;
			_mouse_down_position = GetMouseMsg(para).point.x;
			_mouse_down_split_line_position = GetStyle().split_line._position.AsSigned();
		}
		break;
	case Msg::LeftUp:
		if (_is_mouse_dragging) {
			_is_mouse_dragging = false;
			ReleaseCapture();
		}
		break;
	case Msg::MouseMove:
		if (_is_mouse_dragging) {
			ValueTag& split_line_position = GetStyle().split_line._position;
			if (split_line_position.IsPixel()) {
				split_line_position = px(_mouse_down_split_line_position + GetMouseMsg(para).point.x - _mouse_down_position);
				_is_layout_invalid = true; ContentLayoutChanged();
			} else if (split_line_position.IsPercent() && GetClientSize().width != 0) {
				split_line_position = pct(_mouse_down_split_line_position +
										  (GetMouseMsg(para).point.x - _mouse_down_position) * 100 / GetClientSize().width);
				_is_layout_invalid = true; ContentLayoutChanged();
			}
		} else {
			if (_region_split_line.Contains(GetMouseMsg(para).point)) {
				if (_mouse_position != MousePosition::SplitLine) {
					_mouse_position = MousePosition::SplitLine;
					SetCursor(Cursor::Resize0);
				}
			} else {
				if (_mouse_position != MousePosition::Other) {
					_mouse_position = MousePosition::Other;
					SetCursor(GetStyle().cursor._cursor);
				}
			}
		}
		break;
	case Msg::MouseLeave:
		_mouse_position = MousePosition::None;
		break;
	}
}


END_NAMESPACE(WndDesign)