#pragma once

#include "../common/uncopyable.h"
#include "geometry.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


// Using Win32 GDI APIs to calculate union regions.
class Region : Uncopyable {
private:
	using HANDLE = void*;
	HANDLE rgn;

public:
	static Region& Temp(Rect rect);

public:
	explicit Region(Rect region = region_empty);
	~Region();

	bool IsEmpty() const;
	void Set(Rect region);
	void Clear() { Set(Rect(0, 0, 0, 0)); }

	void Translate(Vector vector);

	void Swap(Region&& region) { std::swap(rgn, region.rgn); }

	void Union(const Region& region);
	void Intersect(const Region& region);
	void Sub(const Region& region);

	void Union(const Rect& region);
	void Intersect(const Rect& region);
	void Sub(const Rect& region);

	std::pair<Rect, vector<Rect>> GetRect() const;
};


END_NAMESPACE(WndDesign)