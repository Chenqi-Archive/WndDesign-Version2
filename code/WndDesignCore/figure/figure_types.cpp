#include "../layer/background_types.h"
#include "../layer/layer.h"
#include "../geometry/rect_point_iterator.h"
#include "../geometry/geometry_helper.h"

#include "../system/directx/directx_helper.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


//////////////////////////////////////////////////////////
////                background_types.h                ////
//////////////////////////////////////////////////////////

const NullBackground& NullBackground::Get() {
	static NullBackground null_background;
	return null_background;
}

void NullBackground::Clear(Rect region, RenderTarget& target, Vector offset) const {
	target.Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void NullBackground::DrawOn(Rect region, RenderTarget& target, Vector offset, uchar opacity) const {
	ID2D1SolidColorBrush* solid_color_brush;
	hr = target.CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &solid_color_brush);
	target.FillRectangle(Rect2RECT(Rect(point_zero + offset, region.size)), solid_color_brush);
	SafeRelease(&solid_color_brush);
}


///////////////////////////////////////////////////////////
////                      layer.h                      ////
///////////////////////////////////////////////////////////

void LayerFigure::DrawOn(RenderTarget& target, Vector offset) const {
	Rect region_to_draw = Rect(region.point - offset, GetTargetSize(target)).Intersect(region);
	for (RectPointIterator it(RegionToOverlappingTileRange(region_to_draw, layer.GetTileSize())); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Vector tile_offset = ScalePointBySize(tile_id, layer.GetTileSize()) - point_zero;
		Rect region_on_tile = (region_to_draw - tile_offset).Intersect(Rect(point_zero, layer.GetTileSize()));

		const Target& source_target = layer.ReadTile(tile_id);
		if (source_target.HasBitmap()) {
			target.DrawBitmap(
				&source_target.GetBitmap(),
				Rect2RECT(region_on_tile + tile_offset - (region.point - point_zero) + offset),
				composite_effect.opacity,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
				Rect2RECT(region_on_tile)
			);
		} else {
			background.DrawOn(
				region_on_tile + tile_offset,
				target,
				offset,
				composite_effect.opacity
			);
		}
	}
}


END_NAMESPACE(WndDesign)