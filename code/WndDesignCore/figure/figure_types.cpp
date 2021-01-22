#include "figure_types.h"
#include "image.h"
#include "text_block.h"
#include "../layer/background_types.h"
#include "../layer/layer.h"

#include "../system/directx/directx_helper.h"
#include "../system/directx/d2d_api.h"
#include "../system/directx/wic_api.h"
#include "../system/directx/dwrite_api.h"


BEGIN_NAMESPACE(WndDesign)


///////////////////////////////////////////////////////////
////                   figure_base.h                   ////
///////////////////////////////////////////////////////////

const Size GetTargetSize(const RenderTarget& target) {
	return SIZE2Size(target.GetSize());
}


//////////////////////////////////////////////////////////
////                  figure_types.h                  ////
//////////////////////////////////////////////////////////

void Line::DrawOn(RenderTarget& target, Vector offset) const {
	if (width > 0 && !color.IsInvisible()) {
		target.DrawLine(
			Point2POINT(point_zero + offset),
			Point2POINT(point_zero + end + offset),
			&GetSolidColorBrush(color),
			width
		);
	}
}

void Rectangle::DrawOn(RenderTarget& target, Vector offset) const {
	if (border_width > 0 && !border_color.IsInvisible()) {
		target.DrawRectangle(
			Rect2RECT(Rect(point_zero + offset, size)),
			&GetSolidColorBrush(border_color),
			border_width
		);
	}
	if (!fill_color.IsInvisible()) {
		target.FillRectangle(
			Rect2RECT(Rect(point_zero + offset, size)),
			&GetSolidColorBrush(fill_color)
		);
	}
}

void RoundedRectangle::DrawOn(RenderTarget& target, Vector offset) const {
	if (border_width > 0 && !border_color.IsInvisible()) {
		target.DrawRoundedRectangle(
			D2D1::RoundedRect(Rect2RECT(Rect(point_zero + offset, size)), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)),
			&GetSolidColorBrush(border_color),
			border_width
		);
	}
	if (!fill_color.IsInvisible()) {
		target.FillRoundedRectangle(
			D2D1::RoundedRect(Rect2RECT(Rect(point_zero + offset, size)), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)),
			&GetSolidColorBrush(fill_color)
		);
	}
}

void Ellipse::DrawOn(RenderTarget& target, Vector offset) const {
	if (border_width > 0 && !border_color.IsInvisible()) {
		target.DrawEllipse(
			D2D1::Ellipse(Point2POINT(point_zero + offset), static_cast<FLOAT>(radius_x), static_cast<FLOAT>(radius_y)),
			&GetSolidColorBrush(border_color),
			border_width
		);
	}
	if (!fill_color.IsInvisible()) {
		target.FillEllipse(
			D2D1::Ellipse(Point2POINT(point_zero + offset), static_cast<FLOAT>(radius_x), static_cast<FLOAT>(radius_y)),
			&GetSolidColorBrush(fill_color)
		);
	}
}


///////////////////////////////////////////////////////////
////                 geometry_figure.h                 ////
///////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////
////                      image.h                      ////
///////////////////////////////////////////////////////////

void ImageFigure::DrawOn(RenderTarget& target, Vector offset) const {
	if (!region_on_image.IsEmpty() && opacity > 0) {
		target.DrawBitmap(
			&image.GetImageResource().GetD2DBitmap(),
			Rect2RECT(Rect(point_zero + offset, region_on_image.size)),
			Opacity2Float(opacity),
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			Rect2RECT(region_on_image)
		);
	}
}


//////////////////////////////////////////////////////////
////                   text_block.h                   ////
//////////////////////////////////////////////////////////

void TextBlockFigure::DrawOn(RenderTarget& target, Vector offset) const {
	target.DrawTextLayout(
		Point2POINT(point_zero + offset),
		&text_block.LayoutResource().DWriteTextLayout(),
		&GetSolidColorBrush(text_block.DefaultStyle().color),
		D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
	);
}


//////////////////////////////////////////////////////////
////                background_types.h                ////
//////////////////////////////////////////////////////////

void SolidColorBackground::Clear(Rect region, RenderTarget& target, Vector offset) const {
	target.Clear(Color2COLOR(color));
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