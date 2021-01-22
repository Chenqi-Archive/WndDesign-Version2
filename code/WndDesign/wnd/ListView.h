#pragma once

#include "WndObject.h"


BEGIN_NAMESPACE(WndDesign)


class ListView : public WndObject {
public:
	struct Style {
		int max_height;
	};

private:
	vector<uint> row_height;

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
	void RemoveChild(uint row) {
		row_height.erase(row_height.begin() + row);
		// calculate row position.
		// set child region
		// invalidate
	}
};


END_NAMESPACE(WndDesign)