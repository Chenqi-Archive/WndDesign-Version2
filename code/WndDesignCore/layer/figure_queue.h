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
		union {
			// as group begin
			struct {
				uint group_end_index;
				uint figure_index;
				Vector coordinate_offset;
				Rect bounding_region;
			};
			// as group end
			struct {
				uint null_index;   // -1
				uint figure_index;
				mutable Vector prev_offset;
				mutable Rect prev_clip_region;
			};
		};
		bool IsBegin() const { return group_end_index != (uint)-1; }
	};
private:
	vector<FigureContainer> figures;
	vector<FigureGroup> groups;

public:
	const vector<FigureGroup>& GetFigureGroups() const { return groups; }
	const vector<FigureContainer>& GetFigures() const { return figures; }

public:
	uint BeginGroup(Vector coordinate_offset, Rect bounding_region) {
		uint group_begin_index = (uint)groups.size();
		groups.push_back(FigureGroup{ (uint)-1, (uint)figures.size(), coordinate_offset, bounding_region });
		return group_begin_index;
	}
	void EndGroup(uint group_begin_index) {
		groups[group_begin_index].group_end_index = (uint)groups.size();
		groups.push_back(FigureGroup{ (uint)-1, (uint)figures.size(), vector_zero, region_empty });
	}

public:
	void Append(Vector offset, unique_ptr<const Figure> figure) {
		figures.emplace_back(FigureContainer{ offset, std::move(figure) });
	}
	void Append(Vector offset, alloc_ptr<const Figure> figure) {
		Append(offset, unique_ptr<const Figure>(figure));
	}
};


END_NAMESPACE(WndDesign)