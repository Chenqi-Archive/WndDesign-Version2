#pragma once

#include "Wnd.h"
#include "../common/reference_wrapper.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


class ListLayout : public Wnd {
public:
	struct Style : public Wnd::Style {
		struct GridStyle {
			ValueTag height = height_auto;
			ValueTag max_height = height_auto;
			ValueTag min_height = height_auto;
			uint gridline_width = 3;
			Color gridline_color = ColorSet::Black;
		}grid;
	};

private:
	struct RowContainer {
		reference_wrapper<WndObject> wnd;
		uint y;
		uint height;
		ValueTag height_tag;
	};
	vector<RowContainer> _rows;

public:
	void AddChild(WndObject& child, uint row) {
		RegisterChild(child);
		SetChildData(child, row);
		Rect child_region = GetChildRegion(child, GetSize());
		BoundNumberBetweenInterval(child_height, { min_height, max_height });
		row_height.insert(row_height.begin() + row, { child_height });
		// calculate row position
		for (all child windows whose row >= row) {
			Rect child_region = Rect(0, row_height[row].position, GetSizeAsParent().width, child_height);
			child.SetRegionOnParent(child_region);
		}
		// invalidate
	}
	void RemoveChild(WndObject& child) {
		// Check if is child.
		uint row = GetChildData(child);
		RemoveChild(row);
	}
	virtual void OnChildDetach(WndObject& child) override {
		ChildWndContainer& child_container = *GetChildData<ChildWndContainer*>(child);
		Rect child_region = child_container.region;
		_child_wnds.erase(child_container.list_index);
		Invalidate(child_region);
	}
	void RemoveChild(uint row) {
		row_height.erase(row_height.begin() + row);
		// calculate row position.
		// set child region
		// invalidate
	}

private:
	struct HitTestInfo {

	};

	const HitTestInfo HitTestPoint(Point point) {

	}
};


END_NAMESPACE(WndDesign)