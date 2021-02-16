#pragma once

#include "TextBox.h"
#include "../message/timer.h"


BEGIN_NAMESPACE(WndDesign)


class EditBox : public TextBox {
public:
	struct Style : TextBox::Style {
		struct EditStyle {
		public:
			Color _selection_color = Color(ColorSet::DimGray, 0x7f);
			Color _caret_color = ColorSet::DimGray;
			bool _disable_edit = false; // can only select and copy
		public:
			constexpr EditStyle& selection_color(Color selection_color) { _selection_color = selection_color; return *this; }
			constexpr EditStyle& caret_color(Color caret_color) { _caret_color = caret_color; return *this; }
			constexpr EditStyle& disable_edit() { _disable_edit = true; return *this; }
		}edit;
	};

public:
	EditBox(unique_ptr<Style> style, const wstring& text);
	~EditBox();


	//// style ////
protected:
	Style& GetStyle() { return static_cast<Style&>(TextBox::GetStyle()); }
	const Style& GetStyle() const { return static_cast<const Style&>(TextBox::GetStyle()); }
private:
	const Style::EditStyle& GetEditStyle() const { return GetStyle().edit; }
	bool IsEditDisabled() const { return GetEditStyle()._disable_edit; }


	//// text block ////
public:
	using HitTestInfo = TextBlockHitTestInfo;
private:
	uint GetCharacterLength(uint text_position);


	//// painting and composition ////
private:
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const override;


	//// caret ////
private:
	inline static const uint caret_blink_period = 500;
	inline static const uint caret_blink_expire_time = 20000;
	enum class CaretState { Hide, Show, BlinkHide, BlinkShow };
	class CaretTimer : public Timer {
		EditBox& edit_box;
		virtual void OnAlert() override { edit_box.BlinkCaret(); }
	public:
		CaretTimer(EditBox& edit_box) : edit_box(edit_box) {}
	};
private:
	CaretTimer _timer = CaretTimer(*this);
	CaretState _caret_state = CaretState::Hide;
	uint _caret_blink_time = 0;
private:
	bool IsCaretVisible() const { return _caret_state == CaretState::Show || _caret_state == CaretState::BlinkShow; }
	void InvalidateCaretRegion() { NonClientInvalidate(_caret_region + ClientToDisplayOffset()); }
private:
	void HideCaret();
	void StartBlinkingCaret();
	void BlinkCaret();

	// caret position
private:
	static const uint caret_width = 1;
	enum class CaretMoveDirection { Left, Right, Up, Down, Home, End };
private:
	uint _caret_text_position = 0;
	Rect _caret_region = region_empty;
private:
	void UpdateCaret(const HitTestInfo& info);
private:
	void SetCaret(Point mouse_down_position);
	void SetCaret(uint text_position, bool is_trailing_hit);
	void MoveCaret(CaretMoveDirection direction);


	//// selection ////
private:
	uint _mouse_down_text_position = 0;
	uint _selection_begin = 0;
	uint _selection_end = 0;
	vector<HitTestInfo> _selection_info;
	Rect _selection_region_union;
private:
	void InvalidateSelectionRegion() { NonClientInvalidate(_selection_region_union + ClientToDisplayOffset()); }
private:
	bool HasSelection() const { return _selection_end > _selection_begin; }
	void DoSelection(Point mouse_move_position);
	void ClearSelection();


	//// keyboard input ////
private:
	void Insert(wchar ch);
	void Insert(const wstring& str);
	void Delete(bool is_backspace);


	//// ime input ////
private:
	uint _ime_composition_begin = 0;
	uint _ime_composition_end = 0;
private:
	void ImeCompositionBegin();
	void ImeComposition(const wstring& composition_string);


	//// clipboard ////
private:
	void Cut();
	void Copy();
	void Paste();


	//// message handling ////
private:
	bool _has_mouse_down = false;
	bool _has_ctrl_down = false;
	Point _mouse_down_position = point_zero;
protected:
	virtual bool Handler(Msg msg, Para para) override;
};


END_NAMESPACE(WndDesign)