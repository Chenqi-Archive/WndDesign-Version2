#pragma once

#include "wnd_base_interface.h"
#include "ObjectBase.h"
#include "wnd_layer.h"

#include <list>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::list;
using std::unique_ptr;

using std::make_unique;


class WndBase : public IWndBase, public Uncopyable {
	// Helper functions for casting.
	static WndBase& ConvertWnd(IWndBase& wnd) { return static_cast<WndBase&>(wnd); }
	static const WndBase& ConvertWnd(const IWndBase& wnd) { return static_cast<const WndBase&>(wnd); }


	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	WndBase(ObjectBase& object) : _object(object) {}
	virtual ~WndBase() override {
		DetachFromParent();
	}


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	Size _size;

private:
	const Rect CalculatePosition(Size parent_size) {
		Rect region = _object.CalculatePosition(parent_size);
		_size = region.size;
		return region;
	}
	const CompositeStyle GetCompositeStyle() const { return _object.GetCompositeStyle(); }

public:
	virtual void PositionUpdated() override {
		if (_parent != nullptr) { _parent->UpdateChildPosition(*this); }
	}
	virtual void CompositeStyleUpdated() override {
		if (_parent != nullptr) { _parent->UpdateChildCompositeStyle(*this); }
	}

	const Size GetSize() const { return _size; }


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
public:
	class ChildWnd : Uncopyable{
	private:
		WndBase& wnd;
		Rect region;  // relative to parent window.
	public:
		ChildWnd(WndBase& wnd, WndLayer& layer, Rect region) : wnd(wnd), layer_index(layer), region(region) {

		}
		~ChildWnd() {

		}
		operator WndBase& () const { return wnd; }
	};
private:
	list<ChildWnd> _child_wnds;

private:
	bool IsMyChild(const WndBase& child_wnd) const { return child_wnd._parent == this; }

public:
	virtual void AddChild(IWndBase& child_wnd) override;
	virtual void RemoveChild(IWndBase& child_wnd) override;

	virtual void UpdateChildPosition(WndBase& child_wnd);   // overrided by Desktop
	virtual void UpdateChildCompositeStyle(WndBase& child_wnd);   // overrided by Desktop


	///////////////////////////////////////////////////////////
	////                   Parent Window                   ////
	///////////////////////////////////////////////////////////
private:
	ref_ptr<WndBase> _parent;
	list<ChildWnd>::iterator _parent_list_index;
	Rect _visible_region;

public:
	bool HasParent() const { return _parent != nullptr; }

	// Called by new parent window.
	void SetParent(ref_ptr<WndBase> parent, list<ChildWnd>::iterator parent_iterator) {
		DetachFromParent();
		_parent = parent; _parent_list_index = parent_iterator;
	}
	// Called by old parent window.
	void ClearParent() { 
		_parent = nullptr; 
		_parent_list_index = {}; 
		_visible_region = region_empty; 
	}
private:
	// Called by myself.
	void DetachFromParent() { 
		if (HasParent()) { _parent->RemoveChild(*this); } 
	}

public:
	const Rect GetVisibleRegion() { return _visible_region; }
	void RefreshVisibleRegion(Rect visible_region) {
		_visible_region = visible_region;

	}


	///////////////////////////////////////////////////////////
	////                       Layer                       ////
	///////////////////////////////////////////////////////////
private:
	unique_ptr<MultipleWndLayer> _base_layer;
	list<SingleWndLayer> _top_layers;

	MultipleWndLayer& BaseLayer() {
		if (_base_layer == nullptr) { _base_layer = make_unique<MultipleWndLayer>(); }
		return *_base_layer;
	}

	list<SingleWndLayer>& TopLayers() { return _top_layers; }

	bool IsBaseLayer(const WndLayer& layer) const { return _base_layer.get() == &layer; }


	///////////////////////////////////////////////////////////
	////                       Paint                       ////
	///////////////////////////////////////////////////////////
private:
	void PositionUpdated(Rect region);

public:
	// Called by Layer (parent layer).
	const FigureQueue GetFigureQueue(Rect region);


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
private:
	ObjectBase& _object;

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