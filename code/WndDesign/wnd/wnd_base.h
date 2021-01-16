#pragma once

#include "../layer/layer.h"
#include "../layer/background.h"
#include "../geometry/region.h"
#include "../common/reference_wrapper.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;

class WndObject;

class WndBase {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
private:
	WndObject& _object;

public:
	WndBase(WndObject& object);
	~WndBase();


	///////////////////////////////////////////////////////////
	////                   Parent Window                   ////
	///////////////////////////////////////////////////////////
private:
	ref_ptr<WndBase> _parent;
	list<ref_ptr<WndBase>>::iterator _index_on_parent;
public:
	bool HasParent() const { return _parent != nullptr; }
	WndObject& GetParent() const { 
		if (!HasParent()) { throw std::invalid_argument("window has no parent"); }
		return _parent->_object;
	}
private:
	/* called by new parent window */
	void SetParent(ref_ptr<WndBase> parent, list<ref_ptr<WndBase>>::iterator index_on_parent);
	/* called by old parent window */
	void ClearParent();
	/* called by myself */
	void DetachFromParent();


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<ref_ptr<WndBase>> _child_wnds;
public:
	void AddChild(WndBase& child_wnd);
	void RemoveChild(WndBase& child_wnd);


	//////////////////////////////////////////////////////////
	////                      Region                      ////
	//////////////////////////////////////////////////////////
private:
	// The entire region of the window.
	Rect _accessible_region;
	// The display region's offset, in my coordinates, size is determined by parent window.
	// Rect(display_region_offset, display_region.size) must be contained in accessible_region.
	Point _display_offset;
	// The region shown on parent window, in parent's coordinates.
	Rect _display_region;
	// The region that is cached on parent layer and must be composited, also used for child layer caching.
	//   (Parent's cached region relative to child is child's visible region.)
	Rect _visible_region;
public:
	const Rect GetAccessibleRegion() const { return _accessible_region; }
	void SetAccessibleRegion(Rect accessible_region);
	void SetDisplayOffset(Point display_offset);
	void SetDisplayRegion(Rect display_region);
public:
	// point_on_parent + offset_from_parent = point_on_myself
	const Vector OffsetFromParent() const { return _display_offset - _display_region.point; }


	//// background ////
private:
	reference_wrapper<const Background> _background;
public:
	void SetBackground(const Background& background) { _background = background; }


	//// layer management ////
private:
	unique_ptr<Layer> _layer;
private:
	bool HasLayer() const { return _layer != nullptr; }
	Layer& GetLayer() const { return *_layer; }
public:
	void AllocateLayer() { if (!HasLayer()) { _layer.reset(new Layer(_accessible_region.size)); } }

private:
	const Rect GetCachedRegion() const { return HasLayer() ? GetLayer().GetCachedRegion() : _visible_region; }
	const Rect GetVisibleRegion() const { return HasParent() ? _parent->GetCachedRegion() + OffsetFromParent() : region_empty; }
public:
	void SetVisibleRegion(Rect visible_region);


	///////////////////////////////////////////////////////////
	////                    Composition                    ////
	///////////////////////////////////////////////////////////

	//// window depth ////
private:
	uint _depth;  // used for determining the redraw queue priority.
public:
	bool HasDepth() const { return _depth > 0; }
	uint GetDepth() const { return _depth; }
	void SetDepth(uint depth);


	//// redraw queue ////
private:
	list<ref_ptr<WndBase>>::iterator _redraw_queue_index;
public:
	bool HasRedrawQueueIndex() const { return _redraw_queue_index != list<ref_ptr<WndBase>>::iterator(); }
	const list<ref_ptr<WndBase>>::iterator GetRedrawQueueIndex() const { _redraw_queue_index; }
	void SetRedrawQueueIndex(list<ref_ptr<WndBase>>::iterator index = list<ref_ptr<WndBase>>::iterator()) { _redraw_queue_index = index; }
private:
	void JoinRedrawQueue();
	void LeaveRedrawQueue();


	//// invalid region ////
private:
	Region _invalid_region;
private:
	/* called by child window when child has updated invalid region */
	void Invalidate(const Region& region);
public:
	void Invalidate(Rect region);
	/* called by redraw queue at commit time */
	void UpdateInvalidRegion();
	/* called by parent window (WndObject) */
	void Composite(FigureQueue& figure_queue, Rect parent_invalid_region) const;


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