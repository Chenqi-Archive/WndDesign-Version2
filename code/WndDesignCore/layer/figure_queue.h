#pragma once

#include "../figure/figure_base.h"
#include "../common/uncopyable.h"
#include "../layer/composite_effect.h"

#include <vector>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unique_ptr;
using std::vector;


class FigureQueue : public Uncopyable {
private:
	friend class RedrawQueue;
	FigureQueue() {}
	~FigureQueue() {}
	void Clear() {
		figures.clear();
		offset = vector_zero;
		assert(CheckGroupOffsetStack());
		groups.clear();
	}


public:
	struct FigureContainer {
		Vector offset;
		unique_ptr<const Figure> figure;
	};
private:
	vector<FigureContainer> figures;
public:
	const vector<FigureContainer>& GetFigures() const { return figures; }

	void Append(Point offset, unique_ptr<const Figure> figure) {
		figures.emplace_back(FigureContainer{ offset - point_zero + this->offset, std::move(figure) });
	}
	void Append(Point offset, alloc_ptr<const Figure> figure) {
		Append(offset, unique_ptr<const Figure>(figure));
	}


private:
	Vector offset = vector_zero;
	vector<Vector> group_offset_stack;
public:
	bool CheckGroupOffsetStack() const { return group_offset_stack.empty(); }

	const Vector PushOffset(Vector offset) { this->offset += offset; return offset; }
	void PopOffset(Vector offset) { this->offset -= offset; }


public:
	struct FigureGroup {
		union {
			// as group begin
			struct {
				uint group_end_index;
				uint figure_index;
				Vector coordinate_offset;
				Rect bounding_region;
				CompositeEffect composite_effect;
			};
			// as group end
			struct {
				uint null_index;   // -1
				uint figure_index;
				mutable Vector prev_offset;
				mutable Rect prev_clip_region;
				mutable bool composite_effect_nontrivial;
			};
		};
		bool IsBegin() const { return group_end_index != (uint)-1; }
	};
private:
	vector<FigureGroup> groups;
public:
	const vector<FigureGroup>& GetFigureGroups() const { return groups; }

	uint BeginGroup(Vector coordinate_offset, Rect bounding_region, CompositeEffect composite_effect = {}) {
		uint group_begin_index = (uint)groups.size();
		groups.push_back(FigureGroup{ (uint)-1, (uint)figures.size(), coordinate_offset + offset, bounding_region, composite_effect});
		group_offset_stack.push_back(offset); offset = vector_zero;
		return group_begin_index;
	}
	void EndGroup(uint group_begin_index) {
		groups[group_begin_index].group_end_index = (uint)groups.size();
		offset = group_offset_stack.back(); group_offset_stack.pop_back();
		groups.push_back(FigureGroup{ (uint)-1, (uint)figures.size(), vector_zero, region_empty, {}});
	}
};


END_NAMESPACE(WndDesign)