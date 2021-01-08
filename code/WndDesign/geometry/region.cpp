#include "region.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)

static Region& TempRegion() {
    static Region region(Rect(0,0,0,0));
    return region;
}


Region::Region(Rect region) {
    rgn = CreateRectRgn(region.left(), region.top(), region.right(), region.bottom());
}

Region::~Region() {
    DeleteObject(rgn);
}

bool Region::IsEmpty() const {
    // return OffsetRgn(static_cast<HRGN>(rgn), 0, 0) == NULLREGION;
    return CombineRgn(static_cast<HRGN>(rgn), static_cast<HRGN>(rgn), NULL, RGN_COPY) == NULLREGION;
}

void Region::Set(Rect region) {
    SetRectRgn(static_cast<HRGN>(rgn), region.left(), region.top(), region.right(), region.bottom());
}

void Region::Translate(Vector vector) {
    OffsetRgn(static_cast<HRGN>(rgn), vector.x, vector.y);
}

void Region::Union(const Region& region) {
    CombineRgn(static_cast<HRGN>(rgn), static_cast<HRGN>(rgn), static_cast<HRGN>(region.rgn), RGN_OR);
}

void Region::Intersect(const Region& region) {
    CombineRgn(static_cast<HRGN>(rgn), static_cast<HRGN>(rgn), static_cast<HRGN>(region.rgn), RGN_AND);
}

void Region::Sub(const Region& region) {
    CombineRgn(static_cast<HRGN>(rgn), static_cast<HRGN>(rgn), static_cast<HRGN>(region.rgn), RGN_DIFF);
}

void Region::Union(const Rect& region) {
    Region& temp = TempRegion();
    temp.Set(region);
    Union(temp);
}

vector<Rect> Region::GetRect() const {
    int size = GetRegionData(static_cast<HRGN>(rgn), 0, NULL);
    char* buffer = new char[size];
    GetRegionData(static_cast<HRGN>(rgn), size, (LPRGNDATA)buffer);
    RGNDATA& data = *(LPRGNDATA)buffer;
    static_assert(sizeof(RECT) == sizeof(Rect));  // In-place convert RECT to Rect.
    vector<Rect> regions((Rect*)(buffer + data.rdh.dwSize), (Rect*)(buffer + data.rdh.dwSize) + data.rdh.nCount);
    delete[] buffer;
    for (auto& region : regions) {
        RECT* rect = reinterpret_cast<RECT*>(&region);
        assert(rect->right >= rect->left && rect->bottom >= rect->top);
        region = Rect(
            rect->left, rect->top,
            static_cast<uint>(rect->right - rect->left), static_cast<uint>(rect->bottom - rect->top)
        );
    }
    return regions;
}


END_NAMESPACE(WndDesign)