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
	struct FigureGroup {

	};
private:
	vector<FigureContainer> figures;
	vector<FigureGroup> groups;

public:
	void Append(Vector offset, unique_ptr<const Figure> figure) {
		figures.emplace_back(FigureContainer{ offset, std::move(figure) });
	}
	void Append(Vector offset, alloc_ptr<const Figure> figure) {
		Append(offset, unique_ptr<const Figure>(figure));
	}
	bool IsEmpty() const { return figures.empty(); }

};


END_NAMESPACE(WndDesign)