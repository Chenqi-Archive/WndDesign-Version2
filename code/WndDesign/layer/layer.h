#pragma once

#include "tile_cache.h"
#include "figure_queue.h"
#include "composite_effect.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


class Layer : public Uncopyable {
public:
	Layer(Size window_size) :
		_tile_cache(window_size),
		_active_targets() {
	}


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
	CompositeEffect composite_effect;

	LayerFigure(const Layer& layer, Rect region, CompositeEffect composite_effect) :
		layer(layer), region(region), composite_effect(composite_effect) {
	}

	virtual const Rect GetRegion() const override { return Rect(point_zero, region.size); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override;
};


END_NAMESPACE(WndDesign)