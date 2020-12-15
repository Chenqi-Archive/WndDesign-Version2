#pragma once

#include "../figure/figure.h"

#include <vector>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::vector;


struct _FigureContainer {
	unique_ptr<const Figure> figure;
	Vector initial_offset;
};


class FigureQueue : public vector<_FigureContainer> {
public:
	void append(unique_ptr<const Figure> figure, Vector initial_offset) {
		emplace_back(_FigureContainer{ std::move(figure), initial_offset });
	}
};


END_NAMESPACE(WndDesign)