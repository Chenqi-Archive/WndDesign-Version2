#pragma once

#include "../../WndDesignCore/wnd/WndObjectBase.h"
#include "../style/wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


class WndObject : public WndObjectBase {
public:
	using Style = WndStyle;

public:
	WndObject(unique_ptr<Style> style) : _style(std::move(style)) {
		if (style == nullptr) { throw std::invalid_argument("style can't be nullptr"); }
		SetBackground(_style->background.get());
	}
	~WndObject() {}


	//// window style and region calculation ////
private:
	unique_ptr<Style> _style;
protected:
	Style& GetStyle() const { return *_style; }
protected:
	virtual const Rect CalculateRegionOnParent(Size parent_size) override;
	virtual void OnSizeChange(Rect accessible_region) override;
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) override;
	virtual const wstring GetTitle() const override;

};


END_NAMESPACE(WndDesign)