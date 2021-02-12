#pragma once

#include "WndObject.h"
#include "../geometry/margin.h"
#include "../style/wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


class Wnd : public WndObject {
public:
	using Style = WndStyle;

public:
	Wnd(unique_ptr<Style> style);
	~Wnd();


	//// window style ////
private:
	unique_ptr<Style> _style;
protected:
	Style& GetStyle() { return *_style; }
	const Style& GetStyle() const { return *_style; }
protected:
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) override;
	virtual const wstring GetTitle() const override;
private:
	/* called by parent window when region is specified by parent window */
	virtual void SetRegionStyle(Rect parent_specified_region) override;


	//// non-client and client region ////
private:
	Margin _margin_without_padding;
	Margin _margin;
	Rect _client_region;  // in client_region's coordinates
public:
	// point_on_client_region + GetClientOffset() = point_on_accessible_region
	const Vector GetClientOffset() const {return Vector(_margin.left, _margin.top);}
	const Rect GetClientRegion() const { return _client_region; }
	const Size GetClientSize() const { return _client_region.size; }


	//// layout update ////
private:
	struct InvalidLayout {
		bool region_on_parent;
		bool margin;
		bool client_region;
		bool content_layout;
	};
	InvalidLayout _invalid_layout;
	virtual bool HasInvalidLayout() const override {
		return _invalid_layout.region_on_parent || _invalid_layout.margin || 
			_invalid_layout.client_region || _invalid_layout.content_layout;
	}
	WndObject::InvalidateLayout;
protected:
	void RegionOnParentChanged() { _invalid_layout.region_on_parent = true; InvalidateLayout(); }
	void MarginChanged() { _invalid_layout.margin = true; InvalidateLayout(); }
	void ClientRegionChanged() { _invalid_layout.client_region = true; InvalidateLayout(); }
	void ContentLayoutChanged() { _invalid_layout.content_layout = true; InvalidateLayout(); }

private:
	virtual void OnAttachToParent() override { RegionOnParentChanged(); }
	/* called by reflow queue for the first time to check if parent's layout may be influenced */
	// returns true if region_on_parent may change, and reflow queue will notify parent window.
	virtual bool MayRegionOnParentChange() override;
	/* called by reflow queue when finally update layout */
	virtual void UpdateLayout() override;
	/* called by parent window when parent is updating */
	virtual const Rect UpdateRegionOnParent(Size parent_size) override;

	bool UpdateScrollbar();
	const Size UpdateMarginAndClientRegion(Size display_size);
	void UpdateClientRegion(Size displayed_client_size);
	virtual const Rect UpdateContentLayout(Size client_size) { return Rect(point_zero, client_size); }


	//// child windows ////
protected:
	void SetChildRegion(WndObject& child, Rect region) {
		WndObject::SetChildRegion(child, region + GetClientOffset());
	}
	const Rect GetChildRegion(WndObject& child) {
		return WndObject::GetChildRegion(child) - GetClientOffset();
	}


	//// painting and composition ////
protected:
	void CompositeChild(const WndObject& child, FigureQueue& figure_queue, Rect invalid_client_region) const {
		WndObject::CompositeChild(child, figure_queue, GetClientOffset(), invalid_client_region + GetClientOffset());
	}
	void Invalidate(Rect invalid_client_region) {
		WndObject::Invalidate(invalid_client_region + GetClientOffset());
	}
private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const override /*final*/;
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {}
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const override;


	//// message handling ////
private:
	class Border;
	Border& GetBorder() {}
	Scrollbar& GetScrollbar() {}

	enum class MouseTrackInfo { None, Border, Scrollbar, Client, Child } _mouse_track_info;
	WndObject* _mouse_track_child;

	struct MouseTrackInfo2 {
		enum { None, Border, Scrollbar, Client, Child } type;
		union {
			ref_ptr<WndObject> child;
		};
	};

	struct ChildElementInfo : MouseTrackInfo2 {};

	MouseTrackInfo2 _mouse_capture;
	MouseTrackInfo2 _mouse_track;

	// initialize style with wnd to set 
	Style(Wnd& wnd) {

	}

	void SendChildElementMessage(ChildElementInfo child_element, Msg msg, Para para) {
		switch (child_element) {
			// reset parameter position.
		case MouseTrackInfo::Border: GetBorder().Handler(display_size, border_style, msg, para); break;
		case MouseTrackInfo::Scrollbar: GetScrollbar().Handler(msg, para); break;
		case MouseTrackInfo::Client: ClientHandler(msg, para); break;
		case MouseTrackInfo::Child: _mouse_track_child->NonClientHandler(msg, para); break;
		}
	}

	void UpdateMouseTrack(MouseTrackInfo mouse_track_info, ref_ptr<WndObject> mouse_track_child = nullptr) {
		if (_mouse_track_info == mouse_track_info && _mouse_track_child == mouse_track_child) { return; }
		// Send Msg::MouseLeave to previous tracked element.
		switch (_mouse_track_info) {
		case MouseTrackInfo::Border: GetBorder().Handler(Msg::MouseLeave, nullmsg);break;
		case MouseTrackInfo::Scrollbar: GetScrollbar().Handler(Msg::MouseLeave, nullmsg);break;
		case MouseTrackInfo::Client: ClientHandler(Msg::MouseLeave, nullmsg);break;
		case MouseTrackInfo::Child: _mouse_track_child->NonClientHandler(Msg::MouseLeave, nullmsg); break;
		}
		if (mouse_track_info == MouseTrackInfo::Child) { assert(mouse_track_child != nullptr); }
		_mouse_track_info = mouse_track_info;
		_mouse_track_child = mouse_track_child;
		// Send Msg::MouseEnter to new tracked element.
		switch (_mouse_track_info) {
		case MouseTrackInfo::Border: GetBorder().Handler(Msg::MouseEnter, nullmsg); break;
		case MouseTrackInfo::Scrollbar: GetScrollbar().Handler(Msg::MouseEnter, nullmsg); break;
		case MouseTrackInfo::Client: ClientHandler(Msg::MouseEnter, nullmsg); break;
		case MouseTrackInfo::Child: _mouse_track_child->NonClientHandler(Msg::MouseEnter, nullmsg); break;
		}
	}

	void NonClientHandler2(Msg msg, Para para) {
		if (IsMouseMsg(msg) || msg == Msg::MouseEnter) {
			if (HasCapture()) {
				UpdateMouseTrack(_mouse_capture);
				SendChildElementMessage(_mouse_capture_element, msg, para + offset);
				return;
			}
			if (GetBorder().HitTest(display_size, point)) {
				UpdateMouseTrack(MouseTrackInfo::Border);
				GetBorder().Handler(msg, para);
				return;
			}
			if (GetScrollbar().HitTest(display_size, point)) {
				UpdateMouseTrack(MouseTrackInfo::Scrollbar);
				GetScrollbar().Handler(msg, para);
				return;
			}
			if (WndObject* child = ClientHitTestChild(point)) {
				UpdateMouseTrack(MouseTrackInfo::Child, child);
				child->NonclientHandler(msg, para);
			} {
				UpdateMouseTrack(MouseTrackInfo::Client);
				ClientHandler();
			}

			do {
				if (HasCapture()) {
					selected_element = _mouse_capture;
				}

			if (GetScrollbar().HitTest(display_size, point)) {
				selected_element = Scrollbar;
				point += offset;
			}
				break;

			} while (false);
			UpdateMouseTrack(MouseTrackInfo::Scrollbar);
			SendChildElementMessage(selected_element);
			return;

		}
		if (msg == Msg::MouseLeave) {
			UpdateMouseTrack(MouseTrackInfo::None);
		}
		if (msg == Msg::LoseCapture) {
			UpdateCapture(None);
		}
	}

private:
	void UpdateCapture(MouseTrackInfo mouse_capture) {
		if (_mouse_capture == mouse_capture) { return; }

		// old lose capture
		SendNotifyMessage(_mouse_capture_element, Msg::LoseCapture, nullmsg);

		// new set capture
		_mouse_capture == mouse_capture;

		if (_mouse_capture != None) {
			GetParent().SetChildCapture();  // virtual function!
		}
	}

private:
	friend class Border;
	void SetBorderCapture() {}
	void DragBorder(BorderPosition, Vector);

	friend class Scrollbar;
	void SetScrollbarCapture() {}
	void SetScrollPosition() {}


public:
	void SetClientCapture() { SetCapture(MouseTrackInfo::Client); }
private:
	virtual void SetChildCapture(WndObject& child) override { SetCapture(MouseTrackInfo::Child, &child); }


private:
	void OnMouseMsg(Msg msg, MouseMsg& mouse_msg) {

	}

	void OnKeyboardMsg(Msg msg, Para para) {

	}

private:
	virtual bool NonClientHitTest(Size display_size, Point point) const;
	virtual bool NonClientHandler(Msg msg, Para para) override;
	virtual bool Handler(Msg msg, Para para) override final;
	virtual bool ClientHandler(Msg msg, Para para) { return true; }
};


END_NAMESPACE(WndDesign)