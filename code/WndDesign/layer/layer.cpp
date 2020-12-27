#include "layer.h"
#include "../geometry/geometry_helper.h"
#include "../geometry/rect_point_iterator.h"
#include "../system/directx/d2d.h"
#include "../system/directx/directx_helper.h"


BEGIN_NAMESPACE(WndDesign)


bool Layer::SetAccessibleRegion(Rect accessible_region, Rect visible_region) {
	_accessible_region = accessible_region;
	bool need_redraw = _tile_cache.ResetTileSize(_accessible_region.size);
	SetCachedRegion(visible_region);
	return need_redraw;
}

void Layer::ClearRegion(Rect region) {
	auto device_context = D2DDeviceContext();

	for (RectPointIterator it(RegionToOverlappingTileRange(region, GetTileSize())); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Point tile_origin = ScalePointBySize(tile_id, GetTileSize());
		Rect region_on_tile = (region - (tile_origin - point_zero)).Intersect(Rect(point_zero, GetTileSize()));
		Target& target = _tile_cache.WriteTile(tile_id, *this);
		if (region_on_tile == Rect(point_zero, GetTileSize())) {
			// The region overlaps the entire tile, just clear the tile.
			target.Clear();
		} else {
			// Draw background on the tile.
			device_context.SetTarget(&target.GetBitmap());
			GetBackground().DrawOn(region_on_tile + (tile_origin - point_zero), 0xFF,
								   static_cast<RenderTarget&>(device_context), region_on_tile.point - point_zero);
		}
	}
}

void Layer::DrawFigureQueue(const FigureQueue& figure_queue, Vector position_offset, Rect bounding_region) {
	auto device_context = D2DDeviceContext();

#error cached_region, begin, 

	for (auto& container : figure_queue) {
		Rect figure_region = container.figure->GetRegion() + container.offset + position_offset;

		Rect region_to_draw = bounding_region.Intersect(figure_region);
		if (region_to_draw.IsEmpty()) { continue; }

		for (RectPointIterator it(RegionToOverlappingTileRange(region_to_draw, GetTileSize())); !it.Finished(); ++it) {
			TileID tile_id = it.Item();
			Point tile_origin = ScalePointBySize(tile_id, GetTileSize());
			Vector figure_offset_to_tile = figure_region.point - tile_origin;
			Target& target = _tile_cache.WriteTile(tile_id, *this);
			device_context.SetTarget(&target.GetBitmap());
			container.figure->DrawOn(static_cast<RenderTarget&>(device_context), figure_offset_to_tile);
		}
	}
}


void LayerFigure::DrawOn(RenderTarget& target, Vector offset) const {
	Rect region_to_draw = Rect(region.point - offset, GetTargetSize(target)).Intersect(region);
	for (RectPointIterator it(RegionToOverlappingTileRange(region_to_draw, layer.GetTileSize())); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Point tile_origin = ScalePointBySize(tile_id, layer.GetTileSize());
		Rect region_on_tile = (region_to_draw - (tile_origin - point_zero)).Intersect(Rect(point_zero, layer.GetTileSize()));
		const Target& source_target = layer._tile_cache.ReadTile(tile_id);
		source_target.DrawOn(tile_origin - point_zero, region_on_tile, layer._composite_effect, target, offset);
	}
}


END_NAMESPACE(WndDesign)