#pragma once

#include "../layer/layer.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;

class WndBase;
class WndBase::ChildWnd;


class WndLayer : public Layer {
private:
	WndBase& _parent;


public:
	WndLayer(WndBase& parent, Rect accessible_region);
	~WndLayer() {}

public:
	const Rect GetParentVisibleRegion() const;
};


class SingleWndLayer : public WndLayer {
private:
	WndBase& _wnd;
	Vector _composite_offset;   // in parent's coordinate
	uint _z_index;

private:
	const Rect VisibleRegionOnLayer() const { return GetParentVisibleRegion() - _composite_offset; }

public:
	SingleWndLayer(Rect composite_region, CompositeEffect composite_effect);
	~SingleWndLayer();

	void SetCompositeRegion(Rect composite_region) {
		SetAccessibleRegion(Rect(point_zero, composite_region.size), VisibleRegionOnLayer());
		_composite_offset = composite_region.point - point_zero;
	}
	void SetCompositeStyle(CompositeEffect composite_effect) {

	}
};


class MultipleWndLayer : public WndLayer {
private:
	Point _current_point;
	Size _composite_region;   // in layer's coordinate.

};


END_NAMESPACE(WndDesign)