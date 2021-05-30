#include "EditBox.h"
#include "DesktopObject.h"
#include "../figure/figure_types.h"
#include "../system/ime.h"
#include "../system/clipboard.h"


BEGIN_NAMESPACE(WndDesign)


EditBox::EditBox(unique_ptr<Style> style, const wstring& text) :
	TextBox(std::move(style), text), _mouse_tracker(*this) {
	OnTextChange();
}

EditBox::~EditBox() {}

void EditBox::OnTextChange() {
	TextBox::OnTextChange();
	const wstring& text = GetText();
	_word_break_iterator.SetText(text.c_str(), (uint)text.length());
}

const Rect EditBox::UpdateContentLayout(Size client_size) {
	const TextBlock& text_block = GetTextBlock();
	Size old_size = text_block.GetSize();
	text_block.AutoResize(client_size);
	if (text_block.GetSize() != old_size) { 
		Invalidate(region_infinite); 
		UpdateCaretRegion(GetTextBlock().HitTestTextPosition(_caret_text_position));
		UpdateSelectionRegion();
	}
	return Rect(point_zero, text_block.GetSize());
}

void EditBox::OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {
	// Draw caret and selection at composite time.
	if (IsCaretVisible()) {
		figure_queue.Append(_caret_region.point + ClientToDisplayOffset(), new Rectangle(_caret_region.size, GetEditStyle()._caret_color));
	}
	if (HasSelection()) {
		for (auto& it : _selection_info) {
			auto& region = it.geometry_region;
			figure_queue.Append(region.point + ClientToDisplayOffset(), new Rectangle(region.size, GetEditStyle()._selection_color));
		}
	}
	// Draw other default components.
	Wnd::OnComposite(figure_queue, display_size, invalid_display_region);
}

void EditBox::HideCaret() {
	if (_caret_state != CaretState::Hide) {
		_caret_state = CaretState::Hide;
		InvalidateCaretRegion();
	}
}

void EditBox::StartBlinkingCaret() {
	if (_caret_state != CaretState::Hide) {
		if (!_timer.IsSet()) {
			_timer.Set(caret_blink_period);
		}
		_caret_blink_time = 0;
	}
}

void EditBox::BlinkCaret() {
	_caret_blink_time += caret_blink_period;
	if (_caret_blink_time >= caret_blink_expire_time) {
		_caret_state = CaretState::Show;
		_timer.Stop();
		return;
	}
	if (_caret_state == CaretState::Hide) {
		_timer.Stop();
	} else if (_caret_state == CaretState::Show || _caret_state == CaretState::BlinkShow) {
		_caret_state = CaretState::BlinkHide;
		InvalidateCaretRegion();
	} else { // _caret_state = CaretState::BlinkHide.
		_caret_state = CaretState::BlinkShow;
		InvalidateCaretRegion();
	}
}

void EditBox::UpdateCaretRegion(const HitTestInfo& info) {
	InvalidateCaretRegion();
	_caret_text_position = info.text_position;
	_caret_region.point = info.geometry_region.point;
	_caret_region.size = Size(caret_width, info.geometry_region.size.height);
	if (info.is_trailing_hit) {
		_caret_text_position += info.text_length;
		_caret_region.point.x += static_cast<int>(info.geometry_region.size.width);
	}
	desktop.CommitReflowQueue(); ScrollIntoView(_caret_region);
	InvalidateCaretRegion();
}

void EditBox::SetCaret(Point mouse_down_position) {
	HitTestInfo info = GetTextBlock().HitTestPoint(mouse_down_position);
	UpdateCaretRegion(info); _caret_state = CaretState::Show;
	ClearSelection();
	_mouse_down_text_position = _caret_text_position;
}

void EditBox::SetCaret(uint text_position, bool is_trailing_hit) {
	HitTestInfo info = GetTextBlock().HitTestTextPosition(text_position);
	info.is_trailing_hit = is_trailing_hit;
	UpdateCaretRegion(info); _caret_state = CaretState::Show;
	ClearSelection();
}

void EditBox::MoveCaret(CaretMoveDirection direction) {
	switch (direction) {
	case CaretMoveDirection::Left:
		if (HasSelection()) {
			SetCaret(_selection_begin, false);
		} else {
			if (_caret_text_position > 0) {
				SetCaret(_caret_text_position - 1, false);
			}
		}
		break;
	case CaretMoveDirection::Right:
		if (HasSelection()) {
			SetCaret(_selection_end, false);
		} else {
			SetCaret(_caret_text_position, true);
		}
		break;
	case CaretMoveDirection::Up:
		SetCaret(_caret_region.Center() - Vector(0, _caret_region.size.height));
		break;
	case CaretMoveDirection::Down:
		SetCaret(_caret_region.Center() + Vector(0, _caret_region.size.height));
		break;
	case CaretMoveDirection::Home:
		SetCaret(Point(0, _caret_region.Center().y));
		break;
	case CaretMoveDirection::End:
		SetCaret(Point(position_max, _caret_region.Center().y));
		break;
	}
}

void EditBox::UpdateSelectionRegion() {
	GetTextBlock().HitTestTextRange(_selection_begin, _selection_end - _selection_begin, _selection_info);
	InvalidateSelectionRegion();
	_selection_region_union = region_empty;
	for (auto& it : _selection_info) {
		_selection_region_union = _selection_region_union.Union(it.geometry_region);
	}
	InvalidateSelectionRegion();
}

void EditBox::DoSelection(Point mouse_move_position) {
	HitTestInfo info = GetTextBlock().HitTestPoint(mouse_move_position);
	UpdateCaretRegion(info);
	_selection_begin = _mouse_down_text_position;
	_selection_end = _caret_text_position;
	if (_selection_begin == _selection_end) { ClearSelection(); return; }
	if (_selection_end < _selection_begin) { std::swap(_selection_begin, _selection_end); }
	UpdateSelectionRegion(); HideCaret();
}

void EditBox::SelectWord() {
	if (_caret_text_position >= GetText().length()) { return; }
	TextRange word_range = _word_break_iterator.Seek(_caret_text_position);
	_selection_begin = word_range.left(); _selection_end = word_range.right();
	UpdateSelectionRegion(); HideCaret();
}

void EditBox::SelectParagraph() {
	const wstring& text = GetText(); uint length = (uint)text.length();
	_selection_begin = _caret_text_position - 1; _selection_end = _caret_text_position;
	while (_selection_begin < length && text[_selection_begin] != L'\n') { _selection_begin--; } 
	while (_selection_end < length && text[_selection_end] != L'\n') { _selection_end++; }
	_selection_begin++; _selection_end++;
	UpdateSelectionRegion(); HideCaret();
}

void EditBox::SelectAll() {
	_selection_begin = 0;
	_selection_end = (uint)GetText().size();
	UpdateSelectionRegion(); HideCaret();
}

void EditBox::ClearSelection() {
	_selection_begin = _selection_end = 0;
	_selection_info.clear();
	InvalidateSelectionRegion();
	_selection_region_union = region_empty;
}

void EditBox::Insert(wchar ch) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		ReplaceText(_selection_begin, _selection_end - _selection_begin, ch);
		SetCaret(_selection_begin + 1, false);
	} else {
		InsertText(_caret_text_position, ch);
		SetCaret(_caret_text_position + 1, false);
	}
}

void EditBox::Insert(const wstring& str) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		ReplaceText(_selection_begin, _selection_end - _selection_begin, str);
		SetCaret(_selection_begin + (uint)str.length(), false);
	} else {
		InsertText(_caret_text_position, str);
		SetCaret(_caret_text_position + (uint)str.length(), false);
	}
}

void EditBox::Delete(bool is_backspace) {
	if (IsEditDisabled()) { return; }
	if (HasSelection()) {
		DeleteText(_selection_begin, _selection_end - _selection_begin);
		SetCaret(_selection_begin, false);
	} else {
		if (is_backspace) {
			if (_caret_text_position == 0) { return; }
			uint previous_caret_position = _caret_text_position;
			SetCaret(previous_caret_position - 1, false);
			uint character_length = previous_caret_position - _caret_text_position;
			DeleteText(_caret_text_position, character_length);
		} else {
			if (_caret_text_position >= GetText().length()) { return; }
			uint character_length = GetCharacterLength(_caret_text_position);
			if (character_length == 0) { return; }
			DeleteText(_caret_text_position, character_length);
		}
	}
}

void EditBox::ImeCompositionBegin() {
	if (IsEditDisabled()) { return; }
	Rect ime_caret_region;
	if (HasSelection()) {
		_ime_composition_begin = _selection_begin;
		_ime_composition_end = _selection_end;
		ime_caret_region.point = _selection_info.front().geometry_region.point;
		ime_caret_region.size = Size(caret_width, _selection_info.front().geometry_region.size.height);
	} else {
		_ime_composition_begin = _caret_text_position;
		_ime_composition_end = _ime_composition_begin;
		ime_caret_region = _caret_region;
	}
	MoveImeWindow(*this, ime_caret_region + ClientToDisplayOffset());
}

void EditBox::ImeComposition(const wstring& composition_string) {
	if (IsEditDisabled()) { return; }
	ReplaceText(_ime_composition_begin, _ime_composition_end - _ime_composition_begin, composition_string);
	_ime_composition_end = _ime_composition_begin + (uint)composition_string.length();
	SetCaret(_ime_composition_end, false);
}

void EditBox::Cut() {
	if (HasSelection()) {
		Copy();
		Delete(false);
	}
}

void EditBox::Copy() {
	if (HasSelection()) {
		SetClipboardData(GetText().substr(_selection_begin, _selection_end - _selection_begin));
	}
}

void EditBox::Paste() {
	if (IsEditDisabled()) { return; }
	wstring str;
	GetClipboardData(str);
	if (!str.empty()) { Insert(str); }
}

void EditBox::Handler(Msg msg, Para para) {
	Wnd::Handler(msg, para);

	switch (_mouse_tracker.Track(msg, para)) {
	case MouseTrackMsg::LeftDown: SetFocus(); SetCaret(GetMouseMsg(para).point); break;
	case MouseTrackMsg::LeftDoubleClick: SelectWord(); break;
	case MouseTrackMsg::LeftTripleClick: SelectParagraph(); break;
	case MouseTrackMsg::LeftDrag: DoSelection(GetMouseMsg(para).point); break;
	case MouseTrackMsg::LeftUp:
	case MouseTrackMsg::MouseMove:
	case MouseTrackMsg::LeftClick:
	default: break;
	}

	switch (msg) {
	case Msg::KeyDown:
		switch (GetKeyMsg(para).key) {
		case Key::Left: MoveCaret(CaretMoveDirection::Left); break;
		case Key::Right: MoveCaret(CaretMoveDirection::Right); break;
		case Key::Up: MoveCaret(CaretMoveDirection::Up); break;
		case Key::Down: MoveCaret(CaretMoveDirection::Down); break;
		case Key::Home: MoveCaret(CaretMoveDirection::Home); break;
		case Key::End: MoveCaret(CaretMoveDirection::End); break;

		case Key::Enter: Insert(L'\n'); break;
		case Key::Tab: Insert(L'\t'); break;

		case Key::Backspace: Delete(true); break;
		case Key::Delete: Delete(false); break;

		case Key::Ctrl: _is_ctrl_down = true; break;
		case Key::Shift: _is_shift_down = true; break;

		case CharKey('A'): if (_is_ctrl_down) { SelectAll(); } break;
		case CharKey('X'): if (_is_ctrl_down) { Cut(); } break;
		case CharKey('C'): if (_is_ctrl_down) { Copy(); } break;
		case CharKey('V'): if (_is_ctrl_down) { Paste(); } break;
		}
		break;
	case Msg::KeyUp:
		switch (GetKeyMsg(para).key) {
		case Key::Ctrl: _is_ctrl_down = false; break;
		case Key::Shift: _is_shift_down = false; break;
		}
		break;
	case Msg::Char:
		if (_is_ctrl_down) { break; }
		if (wchar ch = GetCharMsgFiltered(para); ch != 0) {
			Insert(ch);
		};
		break;

	case Msg::LoseFocus:
		ClearSelection();
		HideCaret();
		break;

		// For IME message.
	case Msg::ImeCompositionBegin: ImeCompositionBegin(); break;
	case Msg::ImeComposition:
	case Msg::ImeCompositionEnd: ImeComposition(GetImeCompositionMsg(para).string); break;
	}

	StartBlinkingCaret();
}


END_NAMESPACE(WndDesign)