#include "figure.h"
#include "image.h"
#include "text_block.h"

#include "../system/directx/d2d.h"
#include "../system/directx/wic.h"
#include "../system/directx/dwrite.h"
#include "../system/directx/directx_helper.h"


BEGIN_NAMESPACE(WndDesign)

class DeviceContext : public ID2D1DeviceContext {};  // Alias for ID2D1DeviceContext.


void Line::DrawOn(DeviceContext& target, Vector offset) const {
	target.DrawLine(
		Point2POINT(begin + offset),
		Point2POINT(end + offset),
		&SolidColorBrush(color),
		width
	);
}

void Rectangle::DrawOn(DeviceContext& target, Vector offset) const {
	target.DrawRectangle(
		Rect2RECT(Rect(point_zero + offset, size)),
		&SolidColorBrush(border_color),
		border_width
	);
	target.FillRectangle(
		Rect2RECT(Rect(point_zero + offset, size)),
		&SolidColorBrush(fill_color)
	);
}

void RoundedRectangle::DrawOn(DeviceContext& target, Vector offset) const {
	target.DrawRoundedRectangle(
		D2D1::RoundedRect(Rect2RECT(Rect(point_zero + offset, size)), radius, radius),
		&SolidColorBrush(border_color),
		border_width
	);
	target.FillRoundedRectangle(
		D2D1::RoundedRect(Rect2RECT(Rect(point_zero + offset, size)), radius, radius),
		&SolidColorBrush(fill_color)
	);
}

void ImageFigure::DrawOn(DeviceContext& target, Vector offset) const {
	Rect region_on_image = this->region_on_image.Intersect(Rect(point_zero, image.GetSize()));
	target.DrawBitmap(
		&image.GetImageResource().GetD2DBitmap(),
		Rect2RECT(Rect(point_zero + offset, region_on_image.size)),
		Opacity2Float(opacity),
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		Rect2RECT(region_on_image)
	);
}

void TextBlockFigure::DrawOn(DeviceContext& target, Vector offset) const {
	target.DrawTextLayout(
		Point2POINT(point_zero + offset),
		&text_block.LayoutResource().DWriteTextLayout(),
		&SolidColorBrush(text_block.DefaultStyle().color),
		D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
	);
}


END_NAMESPACE(WndDesign)
