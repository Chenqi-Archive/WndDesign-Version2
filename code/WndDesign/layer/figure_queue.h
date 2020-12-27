#pragma once

#include "../figure/figure_base.h"

#include <vector>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::vector;


class FigureQueue {
public:
	struct FigureContainer {
		Vector offset;
		unique_ptr<const Figure> figure;
	};

private:
	vector<FigureContainer> figures;

public:
	void append(Vector offset, unique_ptr<const Figure> figure) {
		figures.emplace_back(FigureContainer{ offset, std::move(figure) });
	}
	auto begin() const { return figures.begin(); }
	auto end() const { return figures.begin(); }
};


END_NAMESPACE(WndDesign)