#pragma once

#include "../layer/layer.h"
#include "../geometry/region.h"

#include <list>
#include <set>


BEGIN_NAMESPACE(WndDesign)

using std::list;
using std::set;


class WndBase {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
protected:
	WndBase() {}

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


	///////////////////////////////////////////////////////////
	////                    Composition                    ////
	///////////////////////////////////////////////////////////

	//// static RedrawQueue object ////
private:

	class RedrawQueue {
	private:
		/* redraw queue could use an indexed list */
		set<WndBase&> wnds;


	public:
		set<WndBase&>::iterator AddWnd(const WndBase& wnd);
		void RemoveWnd(set<WndBase&>::iterator it);
		void CommitDraw();
	};

	static RedrawQueue redraw_queue;


	//// window depth ////
private:
	uint _depth = 0;  // used for determining the redrawing priority.
public:
	bool operator<(const WndBase& wnd) const {
		return _depth > wnd._depth;
	}
private:
	void SetDepth(uint depth) {
		_depth = depth;
		// Set depth for child windows.
	}


	//// invalid region ////
private:
	Region _invalid_region;
	set<WndBase&>::iterator _redraw_queue_index;

private:
	void PushRedrawQueue() {

	}
	void Invalidate(Rect region) { 
		_invalid_region.Union(region); 
	}
	void Invalidate(const Region& region) { 
		_invalid_region.Union(region); 
	}


	//// layer management ////
private:
	unique_ptr<Layer> _layer;

	bool HasLayer() const { return _layer != nullptr; }

private:
	void UpdateInvalidRegion() {
		// Draw child windows of the region.
		// Clear the invalid region.
		// Invalidate parent region.
	}
private:
	void UpdateInvalidRegion() {
		if (HasLayer()) {
			// Draw figure queue to layer.
			// (Call ObjectBase)
			// - background
			// - child windows
		}

		// Invalidate region for parent window.

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