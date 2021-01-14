#include "layer.h"
#include "../geometry/geometry_helper.h"
#include "../geometry/rect_point_iterator.h"
#include "../system/directx/directx_helper.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


bool Layer::SetAccessibleRegion(Rect accessible_region, Rect visible_region) {
	_accessible_region = accessible_region;
	bool need_redraw = _tile_cache.ResetTileSize(_accessible_region.size);
	SetCachedRegion(visible_region);
	return need_redraw;
}

void Layer::ClearRegion(Rect region) {
	region = region.Intersect(GetCachedRegion());
	auto& device_context = GetD2DDeviceContext();
	for (RectPointIterator it(RegionToOverlappingTileRange(region, GetTileSize())); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Vector tile_offset = ScalePointBySize(tile_id, GetTileSize()) - point_zero;
		Rect region_on_tile = (region - tile_offset).Intersect(Rect(point_zero, GetTileSize()));
		Target& target = _tile_cache.WriteTile(tile_id);
		bool has_begun = target.BeginDraw(region_on_tile);
		if (!has_begun) { _active_targets.push_back(&target); }
		GetBackground().Clear(region_on_tile + tile_offset,
							  static_cast<RenderTarget&>(device_context),
							  region_on_tile.point - point_zero);
	}
}

void Layer::DrawFigureQueue(const FigureQueue& figure_queue, Vector position_offset, Rect bounding_region) {
	bounding_region = bounding_region.Intersect(GetCachedRegion());
	auto& device_context = GetD2DDeviceContext();
	for (RectPointIterator it(RegionToOverlappingTileRange(bounding_region, GetTileSize())); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Vector tile_offset = ScalePointBySize(tile_id, GetTileSize()) - point_zero;
		Rect region_on_tile = (bounding_region - tile_offset).Intersect(Rect(point_zero, GetTileSize()));
		Target& target = _tile_cache.WriteTile(tile_id);
		bool has_begun = target.BeginDraw(region_on_tile);
		if (!has_begun) { _active_targets.push_back(&target); }
	}
	for (auto& container : figure_queue) {
		Vector figure_offset = container.offset + position_offset;
		Rect region_to_draw = bounding_region.Intersect(container.figure->GetRegion() + figure_offset);
		if (region_to_draw.IsEmpty()) { continue; }
		for (RectPointIterator it(RegionToOverlappingTileRange(region_to_draw, GetTileSize())); !it.Finished(); ++it) {
			TileID tile_id = it.Item();
			Vector tile_offset = ScalePointBySize(tile_id, GetTileSize()) - point_zero;
			Vector figure_offset_to_tile = figure_offset - tile_offset;
			Target& target = _tile_cache.WriteTile(tile_id);
			device_context.SetTarget(&target.GetBitmap());
			container.figure->DrawOn(static_cast<RenderTarget&>(device_context), figure_offset_to_tile);
		}
	}
}

void Layer::CommitDraw() {
	for (auto target : _active_targets) {
		target->EndDraw();
	}
	_active_targets.clear();
}


void LayerFigure::DrawOn(RenderTarget& target, Vector offset) const {
	Rect region_to_draw = Rect(region.point - offset, GetTargetSize(target)).Intersect(region);
	for (RectPointIterator it(RegionToOverlappingTileRange(region_to_draw, layer.GetTileSize())); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Vector tile_offset = ScalePointBySize(tile_id, layer.GetTileSize()) - point_zero;
		Rect region_on_tile = (region_to_draw - tile_offset).Intersect(Rect(point_zero, layer.GetTileSize()));

		const Target& source_target = layer._tile_cache.ReadTile(tile_id);
		if (source_target.HasBitmap()) {
			target.DrawBitmap(
				&source_target.GetBitmap(),
				Rect2RECT(region_on_tile + tile_offset - (region.point - point_zero) + offset),
				composite_effect.opacity,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
				Rect2RECT(region_on_tile)
			);
		} else {
			layer.GetBackground().DrawOn(
				region_on_tile + tile_offset,
				target,
				offset,
				composite_effect.opacity
			);
		}
	}
}


END_NAMESPACE(WndDesign)