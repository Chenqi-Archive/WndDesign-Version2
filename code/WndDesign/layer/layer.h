#pragma once

#include "tile_cache.h"
#include "figure_queue.h"
#include "background.h"
#include "composite_effect.h"

#include "../common/reference_wrapper.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


class Layer : public Uncopyable {
private:
	Rect _accessible_region;
	reference_wrapper<const Background> _background;

	CompositeEffect _composite_effect;

public:
	Layer(const Background& background, const CompositeEffect& composite_effect, Rect accessible_region) :
		_accessible_region(accessible_region),
		_background(background),
		_composite_effect(composite_effect),
		_tile_cache(accessible_region.size) {
	}

	const Background& GetBackground() const { return _background; }
	const Rect GetAccessibleRegion() const { return _accessible_region; }

	void SetBackground(const Background& background) { _background = background; }
	bool SetAccessibleRegion(Rect accessible_region, Rect visible_region);
	void SetCompositeEffect(CompositeEffect composite_effect) { _composite_effect = composite_effect; }


	////////////////////////////////////////////////////////////
	////                       Tiling                       ////
	////////////////////////////////////////////////////////////
private:
	TileCache _tile_cache;

public:
	const Size GetTileSize() const { return _tile_cache.GetTileSize(); }
	void SetCachedRegion(Rect visible_region) { _tile_cache.SetCachedRegion(_accessible_region, visible_region); }
	const Rect GetCachedRegion() const { return _tile_cache.GetCachedRegion(); }


	///////////////////////////////////////////////////////////
	////                      Drawing                      ////
	///////////////////////////////////////////////////////////
private:
	vector<ref_ptr<Target>> _active_targets;

public:
	void ClearRegion(Rect region);	// Clear the region with background.
	void DrawFigureQueue(const FigureQueue& figure_queue, Vector position_offset, Rect bounding_region);
	void CommitDraw();

public:
	// Draw myself on another target. For composition.
	friend struct LayerFigure;
};


struct LayerFigure : Figure {
	const Layer& layer;
	Rect region;

	LayerFigure(const Layer& layer, Rect region = region_infinite) : layer(layer), region(region) {}

	virtual const Rect GetRegion() const override { return Rect(point_zero, region.size); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override;
};


END_NAMESPACE(WndDesign)