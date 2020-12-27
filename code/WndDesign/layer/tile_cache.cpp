#include "tile_cache.h"
#include "layer.h"
#include "tiling_helper.h"
#include "../geometry/geometry_helper.h"
#include "../system/directx/d2d.h"


BEGIN_NAMESPACE(WndDesign)


void TileCache::SwapOut() {
    // Find the tile that falls out of cached region.
	for (auto it = _cache.begin(); it != _cache.end(); ++it) {
        if (!_cached_tile_range.Contains(it->first)) {
            _cache.erase(it);
            return;
        }
	}
    assert(false);
}

void TileCache::SetMaxCapacity(uint max_capacity) {
    _max_capacity = max_capacity;
    if (_max_capacity == 0) { Clear(); } // Release all tiles if the region is empty.
    _cache.reserve(_max_capacity);
}

bool TileCache::ResetTileSize(Size layer_size) {
    Size new_tile_size = CalculateTileSize(layer_size, _tile_size);
    if (new_tile_size != _tile_size) { // If tile size is changed, reset tile size, and clear tile_cache.
        _tile_size = new_tile_size;
        Clear();
        return true;
    }
    return false;
}

void TileCache::SetCachedRegion(Rect accessible_region, Rect visible_region) {
    Rect enlarged_region = accessible_region.Intersect(ScaleRegion(accessible_region.Intersect(visible_region), 2.0));
    _cached_tile_range = RegionToOverlappingTileRange(enlarged_region, _tile_size);
    _cached_region = ScaleRectBySize(_cached_tile_range, _tile_size);
    SetMaxCapacity(_cached_tile_range.Area());
}

const Target& TileCache::ReadTile(TileID tile_id) const {
    if (auto it = _cache.find(tile_id); it != _cache.end()) {
        return it->second;
    }
    return *_tile_read_only;
}

Target& TileCache::WriteTile(TileID tile_id, Layer& layer) {
    auto it = _cache.find(tile_id);
    if (it == _cache.end()) {
        while (_cache.size() > _max_capacity) { SwapOut(); }
        it = _cache.emplace(tile_id, layer).first;
    }
    return it->second;
}


END_NAMESPACE(WndDesign)