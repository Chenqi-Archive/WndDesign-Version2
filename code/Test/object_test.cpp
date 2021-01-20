#include "../WndDesign/wnd/WndObject.h"
#include <vector>
#include <list>

using std::vector;
using std::list;

using namespace WndDesign;


#ifdef _DEBUG
#pragma comment(lib, "..\\build\\x64\\Debug\\WndDesignCore.lib")
#else
#pragma comment(lib, "..\\build\\x64\\Release\\WndDesignCore.lib")
#endif // _DEBUG


// WndDesign WndObject Test
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

class OverlapLayout : public WndObject {
private:
	struct ChildWndContainer {
		WndObject& wnd;
		Rect region;
	};
	list<ChildWndContainer> _children;  // or other spatial indexes.

public:
	void AddChild(WndObject& child) {    // z-index
		RegisterChild(child);
		Rect child_region = child.CalculateRegion(GetSizeAsParent());
		SetChildRegion(child, child_region);
		_children.push_back({ child, child_region });
		Invalidate(child_region);
		sizeof(list<ChildWndContainer>::iterator);
	}
	void RemoveChild(WndObject& child) {
		Rect child_region = child.GetRegionOnParnet();
		UnregisterChild(child);
		Invalidate(child_region);
	}

private:
	virtual void OnSizeChange(Rect accessible_region) override {
		

		Invalidate(accessible_region)
	}
	virtual void OnDisplayRegionChange(Rect accessible_region, Rect display_region) override {}
	virtual void OnCachedRegionChange(Rect accessible_region, Rect cached_region) override {}

private:
	virtual const Rect OnChildRegionChange(WndObject& child, Rect child_region) { return child_region; }
	virtual const Rect CalculateRegionOnParent(Size parent_size) { return region_empty; }
	virtual const Rect CalculateAccessibleRegion(Size parent_size, Size display_region_size) { return Rect(point_zero, display_region_size); }
	
private:
	virtual void OnPaint(FigureQueue& figure_queue, Rect accessible_region, Rect invalid_region) const {}

private:
	virtual bool Handler(Msg msg, Para para) { return true; }
	virtual const WndObject& HitTestChild(Point point) const { return *this; }
	virtual bool HitTest(Point point) const { return true; }
};


// window without child
class FinalWnd : public WndObject {
private:
	WndObject::RegisterChild;
	WndObject::UnregisterChild;
};

class ImageBox : public FinalWnd {
	void f() {
		RegisterChild();
	}
};


int main() {

}