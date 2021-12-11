
	struct MouseTrackInfo {
		enum { None, Border, Scrollbar, Client, Child } type;
		ref_ptr<WndObject> child;
	};
	MouseTrackInfo _mouse_track_info;

	struct HitTestInfo {
		ref_ptr<WndObject> child;
		Point point;
	};

	void SendChildElementMessage(MouseTrackInfo child_element, Msg msg, Para para) {
		switch (child_element.type) {
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

protected:
	void OnChildDetach(WndObject& child) {
		
	}

private:
	void UpdateCapture(MouseTrackInfo mouse_capture) {
		if (_mouse_capture == mouse_capture) { return; }

		// old lose capture
		SendNotifyMessage(_mouse_capture_element, Msg::LoseCapture, nullmsg);
		if (_mouse_capture == None) {
			GetParent()->ReleaseChildCapture();
		}


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
	void ReleaseCapture() { SetCapture(MouseTrackInfo::None);}
	void SetClientCapture() { SetCapture(MouseTrackInfo::Client); }
	void ReleaseClientCapture() { ReleaseCapture(); }
private:
	virtual void SetChildCapture(WndObject& child) override { SetCapture(MouseTrackInfo::Child, &child); }


	void SendChildMessage(WndObject& child, Msg msg, Para para) {
		// Track
		child.NonClientHandler(msg, para);
	}
	void SendClientMessage
