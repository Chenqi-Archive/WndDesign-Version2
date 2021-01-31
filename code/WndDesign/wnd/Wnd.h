#pragma once

#include "WndObject.h"
#include "../style/wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


class Wnd : public WndObject {
public:
	using Style = WndStyle;

public:
	Wnd(unique_ptr<Style> style) : _style(std::move(style)) {
		if (style == nullptr) { throw std::invalid_argument("style can't be null"); }
		SetBackground(_style->background.get());
	}
	~Wnd() {}


	//// window style and region calculation ////
private:
	unique_ptr<Style> _style;
protected:
	Style& GetStyle() const { return *_style; }
protected:
	virtual const Rect CalculateRegionOnParent(Size parent_size) override;
	virtual void OnSizeChange(Rect accessible_region) override;
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) override;
	virtual const wstring& GetTitle() const override;

	struct NonClientRegion {
		ushort left;
		ushort top;
		ushort right;
		ushort bottom;
	};
	const NonClientRegion CalculateNonClientRegion();


	// Store the frame.
private:
	unique_ptr<WndFrame> _frame;
public:
	// usage : parent.AddChild(me.AllocateFrame());
	WndFrame& AllocateFrame(unique_ptr<WndFrame> frame = std::make_unique<WndFrame>());
	WndFrame& GetFrame() const;
};


END_NAMESPACE(WndDesign)