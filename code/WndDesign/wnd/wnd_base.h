#pragma once

#include "../layer/layer.h"
#include "../geometry/region.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;


class WndBase {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
protected:
	WndBase();

	~WndBase() {

		DetachFromParent();

		// get out of the drawing queue.
	}


	///////////////////////////////////////////////////////////
	////                   Parent Window                   ////
	///////////////////////////////////////////////////////////
private:
	ref_ptr<WndBase> _parent;
	list<WndBase&>::iterator _index_on_parent;

protected:
	bool HasParent() const { return _parent != nullptr; }

	// Called by new parent window.
	void SetParent(ref_ptr<WndBase> parent, list<WndBase&>::iterator index_on_parent) {
		DetachFromParent();
		_parent = parent; _index_on_parent = index_on_parent;
	}

	// Called by old parent window.
	void ClearParent() {
		_parent = nullptr;
		_index_on_parent = {};
	}

private:
	// Called by myself.
	void DetachFromParent() {
		if (HasParent()) { _parent->RemoveChild(*this); }
	}


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<WndBase&> _child_wnds;

protected:
	void AddChild(WndBase& child_wnd) {
		

	}
	void RemoveChild(WndBase& child_wnd) {
		if (child_wnd._parent != this) { return; }
		_child_wnds.erase(child_wnd._index_on_parent);
		child_wnd.ClearParent();

		// Clear message tracked windows.
	}


	//////////////////////////////////////////////////////////
	////                      Region                      ////
	//////////////////////////////////////////////////////////

	//// layer management ////
private:
	unique_ptr<Layer> _layer;

	bool HasLayer() const { return _layer != nullptr; }


	//// relative region ////
private:
	Rect _display_region;  // in accessibe_region's coordinates.
	Vector _offset_to_parent;  // in parent's coordinates.

public:
	void SetDisplayRegion(Rect display_region) {

	}

	void SetAccessibleRegion(Rect accessible_region);

	void SetVisibleRegion(Rect visible_region);

	const Rect GetCachedRegion() const;


	///////////////////////////////////////////////////////////
	////                    Composition                    ////
	///////////////////////////////////////////////////////////

	//// window depth ////
private:
	uint _depth = 0;  // used for determining the redraw queue priority.
public:
	bool HasDepth() const { return _depth > 0; }
	uint GetDepth() const { return _depth; }
	void SetDepth(uint depth) {
		if (_depth == depth) { return; }
		// Depth has changed, re-enter redraw queue.
		LeaveRedrawQueue();
		_depth = depth;

		// Set depth for child windows.

		// Enter redraw queue, if depth > 0 && invalid region is not empty.
		if (_depth > 0 && !_invalid_region.IsEmpty()) {

		}
	}


	//// redraw queue ////
private:
	list<WndBase&>::iterator _redraw_queue_index;
public:
	bool HasRedrawQueueIndex() const { return _redraw_queue_index != list<WndBase&>::iterator(); }
	const list<WndBase&>::iterator GetRedrawQueueIndex() const { _redraw_queue_index; }
	void SetRedrawQueueIndex(list<WndBase&>::iterator index = list<WndBase&>::iterator()) { _redraw_queue_index = index; }
private:
	void EnterRedrawQueue();
	void LeaveRedrawQueue();


	//// invalid region ////
private:
	Region _invalid_region;
private:
	void Invalidate(Rect region) {
		region = region.Intersect(GetCachedRegion());
		if (!region.IsEmpty()) {
			_invalid_region.Union(region);
			EnterRedrawQueue();
		}
	}
	void Invalidate(const Region& region) {
		_invalid_region.Union(region); 
		EnterRedrawQueue();
	}
public:
	void UpdateInvalidRegion() {
		if (HasLayer()) {
			// Draw figure queue to layer.
			// (Call ObjectBase)
			// - background
			// - child windows
		}

		// Invalidate region for parent window.

		// Clear invalid region.

	}


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
private:
	ref_ptr<WndBase> _capture_wnd;
	ref_ptr<WndBase> _focus_wnd;
	ref_ptr<WndBase> _last_tracked_wnd;

public:
	void SetCapture();
	void ReleaseCapture();
	void SetFocus();
	void ReleaseFocus();
};


END_NAMESPACE(WndDesign)