#include "figure_types.h"
#include "background_types.h"
#include "image.h"
#include "text_block.h"

#include "../system/directx/directx_helper.h"
#include "../system/directx/d2d_api.h"
#include "../system/directx/wic_api.h"
#include "../system/directx/dwrite_api.h"


BEGIN_NAMESPACE(WndDesign)


// Used to shrink the figure who has border to make sure the border is drawn inside the bounding region.
inline const D2D1_RECT_F ShrinkD2DRectByLength(D2D1_RECT_F rect, float length) {
#pragma message(Remark"The new rect may be ill-formed.")
	return D2D1::RectF(rect.left + length, rect.top + length, rect.right - length, rect.bottom - length);
}


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
	if (!fill_color.IsInvisible()) {
		target.FillRectangle(
			Rect2RECT(Rect(point_zero + offset, size)),
			&GetSolidColorBrush(fill_color)
		);
	}
	if (border_width > 0 && !border_color.IsInvisible()) {
		target.DrawRectangle(
			ShrinkD2DRectByLength(Rect2RECT(Rect(point_zero + offset, size)), border_width / 2),
			&GetSolidColorBrush(border_color),
			border_width
		);
	}
}

void RoundedRectangle::DrawOn(RenderTarget& target, Vector offset) const {
	if (!fill_color.IsInvisible()) {
		target.FillRoundedRectangle(
			D2D1::RoundedRect(Rect2RECT(Rect(point_zero + offset, size)), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)),
			&GetSolidColorBrush(fill_color)
		);
	}
	if (border_width > 0 && !border_color.IsInvisible()) {
		target.DrawRoundedRectangle(
			D2D1::RoundedRect(ShrinkD2DRectByLength(Rect2RECT(Rect(point_zero + offset, size)), border_width / 2), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)),
			&GetSolidColorBrush(border_color),
			border_width
		);
	}
}

void Ellipse::DrawOn(RenderTarget& target, Vector offset) const {
	if (!fill_color.IsInvisible()) {
		target.FillEllipse(
			D2D1::Ellipse(Point2POINT(point_zero + offset), static_cast<FLOAT>(radius_x), static_cast<FLOAT>(radius_y)),
			&GetSolidColorBrush(fill_color)
		);
	}
	if (border_width > 0 && !border_color.IsInvisible()) {
		target.DrawEllipse(
			D2D1::Ellipse(Point2POINT(point_zero + offset), static_cast<FLOAT>(radius_x) - border_width / 2, static_cast<FLOAT>(radius_y) - border_width / 2),
			&GetSolidColorBrush(border_color),
			border_width
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
			reinterpret_cast<ID2D1Bitmap1*>(image.d2d_bitmap),
			Rect2RECT(Rect(point_zero + offset, region_on_image.size)),
			Opacity2Float(opacity),
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			Rect2RECT(region_on_image)
		);
	}
}


//////////////////////////////////////////////////////////
////                   text_layout.h                   ////
//////////////////////////////////////////////////////////

void TextBlockFigure::DrawOn(RenderTarget& target, Vector offset) const {
	target.DrawTextLayout(
		Point2POINT(point_zero + offset),
		&text_block.GetLayout(),
		&GetSolidColorBrush(text_block.GetDefaultStyle().font._color),
		D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
	);
}


END_NAMESPACE(WndDesign)