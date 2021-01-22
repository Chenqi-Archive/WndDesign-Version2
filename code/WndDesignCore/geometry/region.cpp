#include "region.h"
#include "../system/win32_helper.h"


BEGIN_NAMESPACE(WndDesign)


// For internal use.
static Region& TempRegion(Rect rect) {
    static Region region(Rect(0,0,0,0));
    region.Set(rect);
    return region;
}

// Another temp Region for external use.
Region& Region::Temp(Rect rect) {
    static Region region(Rect(0, 0, 0, 0));
    region.Set(rect);
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

void Region::Union(const Rect& region) { Union(TempRegion(region)); }
void Region::Intersect(const Rect& region) { Intersect(TempRegion(region)); }
void Region::Sub(const Rect& region) { Sub(TempRegion(region)); }

std::pair<Rect, vector<Rect>> Region::GetRect() const {
    int size = GetRegionData(static_cast<HRGN>(rgn), 0, NULL);
    char* buffer = new char[size];
    GetRegionData(static_cast<HRGN>(rgn), size, (LPRGNDATA)buffer);
    RGNDATA& data = *(LPRGNDATA)buffer;
    static_assert(sizeof(RECT) == sizeof(Rect));  // In-place convert RECT to Rect.
    vector<Rect> regions((Rect*)(buffer + data.rdh.dwSize), (Rect*)(buffer + data.rdh.dwSize) + data.rdh.nCount);
    delete[] buffer;
    for (auto& region : regions) {
        RECT& rect = reinterpret_cast<RECT&>(region);
        assert(rect.right >= rect.left && rect.bottom >= rect.top);
        region = RECT2Rect(rect);
    }
    return { RECT2Rect(data.rdh.rcBound), regions };
}


END_NAMESPACE(WndDesign)