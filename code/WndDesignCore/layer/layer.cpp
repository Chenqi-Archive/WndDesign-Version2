#include "layer.h"
#include "tiling_helper.h"
#include "../geometry/geometry_helper.h"
#include "../geometry/rect_point_iterator.h"
#include "../system/directx/d2d_api.h"
#include "../system/metrics.h"


BEGIN_NAMESPACE(WndDesign)


// The max visible region'size should be no larger than 4*desktop-size (16*desktop-area).
inline bool IsVisibleRegionSizeValid(Size visible_region_size) {
    static const Size max_visible_region_size = ScaleSizeBySize(GetDesktopSize(), Size(4, 4));
    return visible_region_size.width <= max_visible_region_size.width &&
        visible_region_size.height <= max_visible_region_size.height;
}


Layer::Layer() :
    _tile_size(tile_size_empty),
    _cached_tile_range(region_empty),
    _max_capacity(0),
    _cache(),
    _tile_read_only(std::make_unique<Target>(nullptr)) {
}

Layer::~Layer() {}

void Layer::SwapOut() {
    // Find the tile that falls out of cached region.
    for (auto it = _cache.begin(); it != _cache.end(); ++it) {
        if (!_cached_tile_range.Contains(it->first)) {
            _cache.erase(it);
            return;
        }
    }
    // Control never reaches here, max_capacity always equals the area of cached_tile_range.
    assert(false);
}

void Layer::SetMaxCapacity(uint max_capacity) {
    _max_capacity = max_capacity;
    if (_max_capacity == 0) { _cache.clear(); } // Release all tiles if the region is empty.
    _cache.reserve(_max_capacity);
}

void Layer::ResetTileSize(Size layer_size) {
    Size new_tile_size = CalculateTileSize(layer_size, _tile_size);
    if (new_tile_size == _tile_size) { return; }
    // If tile size is changed, reset tile size, clear cached_region and tile_cache.
    _tile_size = new_tile_size;
    _cached_tile_range = region_empty;
    _cache.clear();
}

void Layer::UpdateCachedTileRegion(Rect accessible_region, Rect visible_region) {
    if (!IsVisibleRegionSizeValid(visible_region.size)) { throw std::out_of_range("visible region's size too large"); }
    Rect enlarged_region = accessible_region.Intersect(ScaleRegion(visible_region, 2.0));
    _cached_tile_range = RegionToOverlappingTileRange(enlarged_region, _tile_size);
    SetMaxCapacity(_cached_tile_range.Area());
}

const Rect Layer::GetCachedTileRegion() {
    return ScaleRectBySize(_cached_tile_range, _tile_size);
}

const Target& Layer::ReadTile(TileID tile_id) const {
    if (auto it = _cache.find(tile_id); it != _cache.end()) {
        return it->second;
    }
    return *_tile_read_only;
}

Target& Layer::WriteTile(TileID tile_id) {
    auto it = _cache.find(tile_id);
    if (it == _cache.end()) {
        while (_cache.size() > _max_capacity) { SwapOut(); }
        it = _cache.emplace(tile_id, _tile_size).first;
    }
    return it->second;
}

void Layer::DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region) {
	RectPointIterator it(RegionToOverlappingTileRange(bounding_region, GetTileSize()));
	for (; !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Vector offset_to_tile = point_zero - ScalePointBySize(tile_id, GetTileSize());
		Target& target = WriteTile(tile_id);
		target.DrawFigureQueue(figure_queue, offset_to_tile, bounding_region + offset_to_tile);
	}
}


END_NAMESPACE(WndDesign)