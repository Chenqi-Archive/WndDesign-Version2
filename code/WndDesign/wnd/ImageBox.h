#pragma once

#include "FinalWnd.h"
#include "../figure/image.h"


BEGIN_NAMESPACE(WndDesign)


class ImageBox : public FinalWnd {
public:
	using Style = FinalWnd::Style;

public:
	ImageBox(unique_ptr<Style> style, unique_ptr<Image> image) :
		FinalWnd(std::move(style)), _image(std::move(image)) {
	}
	~ImageBox() {}


	//// image ////
private:
	unique_ptr<Image> _image;
public:
	Image& GetImage() { return *_image; }
	void SetImage(unique_ptr<Image> image) {
		_image = std::move(image);
		Invalidate(region_infinite); 
		ContentLayoutChanged();
	}
protected:
	virtual const Rect UpdateContentLayout(Size client_size) {
		return Rect(point_zero, _image->GetSize());
	}
	virtual void OnClientPaint(FigureQueue& figure_queue, Rect client_region, Rect invalid_client_region) const override {
		if (_image) {
			figure_queue.Append(point_zero, new ImageFigure(*_image));
		}
	}
};


END_NAMESPACE(WndDesign)