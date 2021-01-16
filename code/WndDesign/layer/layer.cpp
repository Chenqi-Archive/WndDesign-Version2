#include "layer.h"
#include "../geometry/geometry_helper.h"
#include "../geometry/rect_point_iterator.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


bool Layer::SetAccessibleRegion(Rect accessible_region, Rect visible_region) {
	_accessible_region = accessible_region;
	bool need_redraw = _tile_cache.ResetTileSize(_accessible_region.size);
	SetCachedRegion(visible_region);
	return need_redraw;
}

void Layer::DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region) {
	RectPointIterator it(RegionToOverlappingTileRange(bounding_region, GetTileSize()));
	for (; !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Vector offset_to_tile = point_zero - ScalePointBySize(tile_id, GetTileSize());
		Target& target = _tile_cache.WriteTile(tile_id);
		target.DrawFigureQueue(figure_queue, offset_to_tile, bounding_region + offset_to_tile);
	}
}


END_NAMESPACE(WndDesign)