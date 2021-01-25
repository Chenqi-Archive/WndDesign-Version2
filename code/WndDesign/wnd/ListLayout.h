#pragma once

#include "WndObject.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using namespace std;


class ListLayout : public WndObject {
public:
	struct Style : public WndObject::Style {
		struct HeightStyle {

		}height;


	};

private:
	struct RowContainer {
		reference_wrapper<WndObjectBase> wnd;
		uint y;
		uint height;
	};
	vector<uint> _rows;

	const Size GetSizeAsParent() {
		return GetSize();
	}

public:
	void AddChild(WndObject& child, uint row) {
		RegisterChild(child);
		SetChildData(child, row);
		uint child_height = child.CalculateRegion(GetSizeAsParent()).size.height;
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
	virtual void OnChildDetach(WndObjectBase& child) override {
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
};


END_NAMESPACE(WndDesign)