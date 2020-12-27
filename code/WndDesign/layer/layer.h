#pragma once

#include "tile_cache.h"
#include "figure_queue.h"
#include "background.h"
#include "composite_effect.h"


BEGIN_NAMESPACE(WndDesign)

using std::shared_ptr;


class Layer : public Uncopyable {
private:
	Rect _accessible_region;
	shared_ptr<Background> _background;
#error background must not be empty.

	CompositeEffect _composite_effect;

public:
	Layer();
	~Layer();

	Background& GetBackground() const { return *_background; }
	bool SetAccessibleRegion(Rect accessible_region, Rect visible_region);
	const Rect GetAccessibleRegion() const { return _accessible_region; }
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
public:
	void BeginDraw();
	void EndDraw();
	void ClearRegion(Rect region);	// Clear the region with background.
	void DrawFigureQueue(const FigureQueue& figure_queue, Vector position_offset, Rect bounding_region);

public:
	// Draw myself on another target. For composition.
	friend class LayerFigure;
};


struct LayerFigure : Figure {
	const Layer& layer;
	Rect region;

	LayerFigure(const Layer& layer, Rect region = region_infinite) : layer(layer), region(region) {}

	virtual const Rect GetRegion() const override { return Rect(point_zero, region.size); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override;
};


END_NAMESPACE(WndDesign)