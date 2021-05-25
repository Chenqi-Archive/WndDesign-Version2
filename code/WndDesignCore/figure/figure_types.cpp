#include "../layer/background_types.h"
#include "../layer/layer.h"
#include "../geometry/rect_point_iterator.h"
#include "../geometry/geometry_helper.h"

#include "../system/directx/directx_helper.h"
#include "../system/directx/d2d_api.h"


BEGIN_NAMESPACE(WndDesign)


///////////////////////////////////////////////////////////
////                   figure_base.h                   ////
///////////////////////////////////////////////////////////

const Size GetTargetSize(const RenderTarget& target) {
	return SIZE2Size(target.GetSize());
}


//////////////////////////////////////////////////////////
////                background_types.h                ////
//////////////////////////////////////////////////////////

const NullBackground& NullBackground::Get() {
	static NullBackground null_background;
	return null_background;
}

void NullBackground::Clear(Rect region, RenderTarget& target, Vector offset) const {
	target.Clear(Color2COLOR(color_transparent));
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
				0xFF,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
				Rect2RECT(region_on_tile)
			);
		} else {
			background.DrawOn(
				region_on_tile + tile_offset,
				target,
				offset
			);
		}
	}
}


///////////////////////////////////////////////////////////
////                     d2d_api.h                     ////
///////////////////////////////////////////////////////////

inline bool IsCompositeEffectNontrivial(CompositeEffect composite_effect) {
    return composite_effect._opacity != 0xFF;
}

void Target::DrawFigureQueue(const FigureQueue& figure_queue, Vector offset, Rect clip_region) {
    if (!figure_queue.CheckGroupOffsetStack()) { throw std::invalid_argument("figure queue groups mismatch"); }
    ID2D1DeviceContext& device_context = GetD2DDeviceContext(); device_context.SetTarget(&GetBitmap());
    auto& groups = figure_queue.GetFigureGroups();
    auto& figures = figure_queue.GetFigures();
    uint figure_index = 0;
    clip_region = clip_region.Intersect(Rect(point_zero, SIZE2Size(bitmap->GetSize())));
    for (uint group_index = 0; group_index < groups.size(); ++group_index) {
        auto& group = groups[group_index];
        for (; figure_index < group.figure_index; ++figure_index) {
            Vector figure_offset = figures[figure_index].offset + offset;
        #pragma message(Remark"The region of the figure could be cached in the figure queue when it is appended.")
            Rect figure_region = figures[figure_index].figure->GetRegion() + figure_offset;
            if (figure_region.Intersect(clip_region).IsEmpty()) { continue; }
            figures[figure_index].figure->DrawOn(static_cast<RenderTarget&>(device_context), figure_offset);
        }
        if (group.IsBegin()) {
            auto& group_end = groups[group.group_end_index];
            // Calculate new offset and clip region.
            Vector new_offset = offset + group.coordinate_offset;
            Rect new_clip_region = clip_region.Intersect(group.bounding_region + new_offset);
            // Jump to group end if new clip region is empty.
            if (new_clip_region.IsEmpty()) {
                group_index = group.group_end_index;
                figure_index = group_end.figure_index;
                continue;
            }
            // Store old offset and clip region to group end.
            group_end.prev_offset = offset;
            group_end.prev_clip_region = clip_region;
            // Set new offset and clip region.
            offset = new_offset;
            clip_region = new_clip_region;
            if (IsCompositeEffectNontrivial(group.composite_effect)) {
                group_end.composite_effect_nontrivial = true;
                D2D1_LAYER_PARAMETERS layer = {};
                layer.contentBounds = Rect2RECT(clip_region);
                layer.opacity = Opacity2Float(group.composite_effect._opacity);
                device_context.PushLayer(layer, NULL);
            } else {
                group_end.composite_effect_nontrivial = false;
                device_context.PushAxisAlignedClip(Rect2RECT(clip_region), D2D1_ANTIALIAS_MODE_ALIASED);
            }
        } else {
            // Restore to previous offset and clip region.
            offset = group.prev_offset;
            clip_region = group.prev_clip_region;
            if (group.composite_effect_nontrivial) {
                device_context.PopLayer();
            } else {
                device_context.PopAxisAlignedClip();
            }
        }
    }
}


END_NAMESPACE(WndDesign)