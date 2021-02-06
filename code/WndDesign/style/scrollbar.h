#pragma once

#include "../geometry/margin.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::shared_ptr;

class FigureQueue;


class Scrollbar {
public:
	Scrollbar() {}
	virtual ~Scrollbar() {}
	virtual bool HasMargin() const { return false; }
	virtual const Margin GetMargin() const { return {}; }
	virtual bool Update(Rect accessible_region, Rect display_region) { return false; }
	virtual bool HitTest(Point point, Size display_size) { return false; }
	virtual bool Handler(Msg msg, Para para) { return false; }
	virtual void OnPaint(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) {}
};


class DefaultScrollbar : public Scrollbar {
private:
	Margin margin;
public:
	DefaultScrollbar() {}
	virtual ~DefaultScrollbar() override {}
	virtual bool HasMargin() const override { return true; }
	virtual const Margin GetMargin() const override { return margin; }
	virtual bool Update(Rect accessible_region, Rect display_region) override {

	}
	virtual bool HitTest(Point point, Size display_size) override {

	}
	virtual bool Handler(Msg msg, Para para) override {

	}
};

shared_ptr<Scrollbar> GetDefaultScrollbar() {
	static shared_ptr<Scrollbar> scrollbar(new Scrollbar);
	return scrollbar;
}


END_NAMESPACE(WndDesign)