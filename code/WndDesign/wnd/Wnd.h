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
private:
	Size _size_min, _size_max;
protected:
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) override;
	virtual const CompositeEffect GetCompositeEffect() const override { return GetStyle().composite; }
private:
	/* called by parent window when region is specified by parent window */
	virtual void SetRegionStyle(Rect parent_specified_region, Size parent_size) override;
private:
	/* called by border resizer */
	void ResetRegionOnParent(Rect old_window_region, Margin margin_to_extend);
protected:
	BorderResizer& GetBorderResizer() const { return *GetStyle().border._resizer; }
	Scrollbar& GetScrollbar() const { return *GetStyle().scrollbar._resource; }


	//// non-client and client region ////
private:
	Margin _margin_without_padding;
	Margin _margin;
	Rect _client_region;  // in client_region's coordinates
	Vector _client_to_display_offset;
public:
	// point_on_client_region + GetClientOffset() = point_on_accessible_region
	const Vector GetClientOffset() const { return Vector(_margin.left, _margin.top); }
	const Rect GetClientRegion() const { return _client_region; }
	const Size GetClientSize() const { return _client_region.size; }
	const Vector ClientToDisplayOffset() const { return _client_to_display_offset; }
public:
	bool IsScrollable() const;


	//// layout update ////
private:
	struct InvalidLayout {
		bool region_on_parent;
		bool margin;
		bool client_region;
		bool content_layout;
	};
	InvalidLayout _invalid_layout;
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

	void UpdateScrollbar(Rect accessible_region, Rect display_region);
	const Size UpdateMarginAndClientRegion(Size display_size);
	void UpdateClientRegion(Size displayed_client_size);
	virtual const Rect UpdateContentLayout(Size client_size) { return Rect(point_zero, client_size); }

	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region);


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
		Vector offset = figure_queue.PushOffset(vector_zero - GetClientOffset());
		WndObject::CompositeChild(child, figure_queue, invalid_client_region + GetClientOffset());
		figure_queue.PopOffset(offset);
	}
	void Invalidate(Rect invalid_client_region) {
		WndObject::Invalidate(invalid_client_region + GetClientOffset());
	}
private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const override final;
protected:
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const {}
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const override;


	//// message handling ////
private:
	enum class ElementType { None, Border, Scrollbar, Client, /*Child*/ };
private:
	void NotifyElement(ElementType type, Msg msg, Para para);

	// mouse capture
private:
	struct MouseCaptureInfo {
		ElementType _type;
		bool IsNull() const { return _type == ElementType::None; }
		void Update(Wnd& wnd, ElementType type);
	};
	MouseCaptureInfo _mouse_capture_info;
private:
	friend class Scrollbar;
	friend class BorderResizer;
	void SetScrollbarCapture() { _mouse_capture_info.Update(*this, ElementType::Scrollbar); }
	void SetBorderCapture() { _mouse_capture_info.Update(*this, ElementType::Border); }
	void LoseCapture() { _mouse_capture_info.Update(*this, ElementType::None); }
private:
	WndObject::SetCapture;
public:
	void SetCapture() { _mouse_capture_info.Update(*this, ElementType::Client); }

	// mouse track
private:
	struct MouseTrackInfo {
		ElementType _type;
		ref_ptr<WndObject> _child;
		bool IsChild() const { return _child != nullptr; }
		void Update(Wnd& wnd, ElementType type);
		void Update(Wnd& wnd, WndObject& child);
	};
	MouseTrackInfo _mouse_track_info;
private:
	void MouseLeave() { _mouse_track_info.Update(*this, ElementType::None); }
protected:
	virtual void OnChildDetach(WndObject& child) override {
		if (_mouse_track_info._child == &child) { _mouse_track_info.Update(*this, ElementType::None); }
	}

protected:
	virtual bool NonClientHitTest(Size display_size, Point point) const override;
	virtual void NonClientHandler(Msg msg, Para para) override;

protected:
	struct HitTestInfo {
		ref_ptr<WndObject> child = nullptr;
		Point point = point_zero;
	};
protected:
	virtual const HitTestInfo ClientHitTest(Size client_size, Point point) const { return {}; }
	virtual void Handler(Msg msg, Para para);
};


END_NAMESPACE(WndDesign)