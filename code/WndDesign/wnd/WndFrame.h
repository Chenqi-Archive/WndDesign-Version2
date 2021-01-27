#pragma once

#include "Wnd.h"


BEGIN_NAMESPACE(WndDesign)


// Window frame base class that contains a single main child window, 
//   and can be implemented to hold border, scroll bar, menu bar, etc.
class WndFrame : public WndObject {
private:
	ref_ptr<Wnd> _child;
	

public:
	const Size GetClientSize() const {

	}

public:
	void SetChild(Wnd& child) {

	}
	virtual void OnChildDetach(WndObject& child) {

	}

	virtual const Rect CalculateRegionOnParent(Size parent_size) { 
		if (_child == nullptr) { return region_empty; }
		return _child->cal
	}

};


END_NAMESPACE(WndDesign)