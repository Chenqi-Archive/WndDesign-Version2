#pragma once

#include "wnd_base_interface.h"
#include "layer.h"
#include "figure_queue.h"

#include <list>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::list;
using std::unique_ptr;

using std::make_unique;


class WndBase : public IWndBase {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
private:
	ObjectBase& _object;

public:
	WndBase(ObjectBase& object) : _object(object) {}
	virtual ~WndBase() {}

	static WndBase& ConvertWnd(IWndBase& wnd) { return static_cast<WndBase&>(wnd); }


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	Size _size;

	virtual void RegionUpdated() override {
		if (_parent != nullptr) { _parent->UpdateChildRegion(*this); }
	}
	virtual void CompositeStyleUpdated() override {
		if (_parent != nullptr) { _parent->UpdateChildCompositeStyle(*this); }
	}


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
private:
	class ChildWndContainer : Uncopyable{
	private:
		WndBase& wnd;
		Layer& layer;
		list<Layer::ChildWndWrapper>::iterator layer_iterator;
	public:
		ChildWndContainer(WndBase& wnd, Layer& layer, list<Layer::ChildWndWrapper>::iterator layer_iterator = {}) :
			wnd(wnd), layer(layer), layer_iterator(layer_iterator) {
		}
		~ChildWndContainer() {}
		operator WndBase& () { return wnd; }
	};
	list<ChildWndContainer> _child_wnds;

private:
	bool IsMyChild(const WndBase& child_wnd) { return child_wnd._parent == this; }

public:
	virtual void AddChild(IWndBase& child_wnd) override;
	virtual void RemoveChild(IWndBase& child_wnd) override;

	virtual void UpdateChildRegion(WndBase& child_wnd);
	virtual void UpdateChildCompositeStyle(WndBase& child_wnd);


	///////////////////////////////////////////////////////////
	////                   Parent Window                   ////
	///////////////////////////////////////////////////////////
private:
	ref_ptr<WndBase> _parent;
	list<ChildWndContainer>::iterator _parent_iterator;
private:

private:
	void SetParent(ref_ptr<WndBase> parent, list<ChildWndContainer>::iterator parent_iterator);


	///////////////////////////////////////////////////////////
	////                       Layer                       ////
	///////////////////////////////////////////////////////////
private:
	struct LayerWrapper {
		unique_ptr<MultipleWndLayer> base_layer;
		list<SingleWndLayer> top_layers;
	}_layer;

	MultipleWndLayer& BaseLayer() {
		if (_layer.base_layer == nullptr) {
			_layer.base_layer = make_unique<MultipleWndLayer>();
		}
		return *_layer.base_layer;
	}

	list<SingleWndLayer>& TopLayers() {
		return _layer.top_layers;
	}

	bool IsBaseLayer(const Layer& layer) { return _layer.base_layer.get() == &layer; }




	///////////////////////////////////////////////////////////
	////                       Paint                       ////
	///////////////////////////////////////////////////////////
private:
	void RegionUpdated(Rect region);

public:
	// Called by Layer (parent layer).
	const FigureQueue GetFigureQueue(Rect region);


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
private:
	ref_ptr<WndBase> _capture_wnd;
	ref_ptr<WndBase> _focus_wnd;
	ref_ptr<WndBase> _last_tracked_wnd;

public:
	virtual void SetCapture() override;
	virtual void ReleaseCapture() override;
	virtual void SetFocus() override;
	virtual void ReleaseFocus() override;
};


END_NAMESPACE(WndDesign)