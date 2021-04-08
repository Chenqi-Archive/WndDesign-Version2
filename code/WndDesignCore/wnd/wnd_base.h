#pragma once

#include "wnd_base_interface.h"
#include "../common/reference_wrapper.h"
#include "../common/list_iterator.h"
#include "../geometry/region.h"

#include <list>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::list;
using std::unique_ptr;

class Layer;


class WndBase : public IWndBase, public Uncopyable {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
protected:
	WndObject& _object;

public:
	WndBase(WndObject& object);
	~WndBase();

	friend class DesktopBase;
	friend class DesktopWndFrame;


	///////////////////////////////////////////////////////////
	////                   Parent Window                   ////
	///////////////////////////////////////////////////////////
private:
	ref_ptr<WndBase> _parent;
	list<ref_ptr<WndBase>>::iterator _index_on_parent;
public:
	bool HasParent() const { return _parent != nullptr; }
private:
	/* called by new parent window */
	void SetParent(WndBase& parent, list<ref_ptr<WndBase>>::iterator index_on_parent);
	/* called by old parent window */
	void ClearParent();
	/* called by myself */
	void DetachFromParent();


	//// window depth ////
	// Depth is used to determine the redraw queue priority. 
	// For Desktop, depth is 0; for desktop window and its descendants, depth is 1, 2, 3, ...
	//   and for windows who are not descendant of Desktop, depth is -1.
private:
	uint _depth;
private:
	bool IsDepthValid() const { return _depth != -1; }
	uint GetChildDepth() const { return _depth == -1 ? -1 : _depth + 1; }
public:
	void SetDepth(uint depth);
	uint GetDepth() const { return _depth; }


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<ref_ptr<WndBase>> _child_wnds;
private:
	void ClearChild();
public:
	virtual void AddChild(IWndBase& child_wnd) override;
	virtual void RemoveChild(IWndBase& child_wnd) override;


	//////////////////////////////////////////////////////////
	////                      Region                      ////
	//////////////////////////////////////////////////////////
private:
	Rect _accessible_region;
	Vector _display_offset;
	Rect _region_on_parent;
	Rect _cached_region;
private:
	bool UpdateDisplayOffset(Vector display_offset);
public:
	// point_on_parent + offset_from_parent = point_on_myself
	const Vector OffsetFromParent() const { return _display_offset - (_region_on_parent.point - point_zero); }
	const Vector GetDisplayOffset() const { return _display_offset; }
	virtual const Rect GetAccessibleRegion() const override { return _accessible_region; }
	virtual const Rect GetDisplayRegion() const override { return Rect(point_zero + _display_offset, _region_on_parent.size); }
	virtual const Rect GetRegionOnParent() const override { return _region_on_parent; }
	virtual void SetAccessibleRegion(Rect accessible_region) override;
	virtual const Vector SetDisplayOffset(Vector display_offset) override;  // returns the display_offset change.
	virtual void SetRegionOnParent(Rect region_on_parent) override;
private:
	const Rect GetCachedRegion() const { return _cached_region; }
	void ResetVisibleRegion() { SetVisibleRegion(HasParent() ? _parent->GetCachedRegion() : region_empty); }
	void SetVisibleRegion(Rect parent_cached_region);


	//// reflow queue ////
private:
	friend class ReflowQueue;
	list_iterator<ref_ptr<WndBase>> _reflow_queue_index;
private:
	void JoinReflowQueue();
	void LeaveReflowQueue();
	virtual void InvalidateLayout() override { JoinReflowQueue(); }


	//// invalid layout ////
public:
	void MayRegionOnParentChange();
	void UpdateInvalidLayout();


	///////////////////////////////////////////////////////////
	////                    Composition                    ////
	///////////////////////////////////////////////////////////

	//// background ////
private:
	reference_wrapper<const Background> _background;
public:
	virtual void SetBackground(const Background& background) override { _background = background; }


	//// layer management ////
private:
	unique_ptr<Layer> _layer;
private:
	bool HasLayer() const { return _layer != nullptr; }
public:
	virtual void AllocateLayer() override;


	//// redraw queue ////
private:
	friend class RedrawQueue;
	list_iterator<ref_ptr<WndBase>> _redraw_queue_index;
private:
	void JoinRedrawQueue();
	void LeaveRedrawQueue();


	//// invalid region ////
private:
	Region _invalid_region;
private:
	/* called by child window when child has updated invalid region */
	virtual void InvalidateChild(WndBase& child, Region& child_invalid_region);
	virtual void InvalidateChild(IWndBase& child, Rect child_invalid_region) override;
public:
	virtual void Invalidate(Rect region) override;
	/* called by redraw queue at commit time */
	void UpdateInvalidRegion(FigureQueue& figure_queue);
	/* called by parent window (WndObject) , the coordinate space of figure_queue now is parent's client region */
	virtual void Composite(FigureQueue& figure_queue, Rect parent_invalid_region, CompositeEffect composite_effect) const override;


	////////////////////////////////////////////////////////////
	////                  Message Handling                  ////
	////////////////////////////////////////////////////////////

public:
	virtual void SetCapture() override;		   // defined in desktop.cpp
	virtual void ReleaseCapture() override;	   // the same
	virtual void SetFocus() override;		   // the same

private:
	void NotifyDesktopWhenDetached();		   // the same
};


END_NAMESPACE(WndDesign)