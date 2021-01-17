#pragma once

#include "../common/uncopyable.h"
#include "../geometry/geometry.h"
#include "figure_queue.h"
#include "composite_effect.h"

#include <unordered_map>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::unordered_map;
using std::unique_ptr;

class Target;

using TileID = Point;
using TileRange = Rect;


struct TileIDHasher {
	size_t operator()(TileID tile_id) const noexcept {
		static_assert(sizeof(TileID) == sizeof(size_t));   // 64bit
		return *reinterpret_cast<size_t*>(&tile_id);
	}
};


class Layer : public Uncopyable {
public:
	Layer();
	~Layer();


	////////////////////////////////////////////////////////////
	////                       Tiling                       ////
	////////////////////////////////////////////////////////////
private:
	Size _tile_size;
	TileRange _cached_tile_range;
	Rect _cached_region;

	uint _max_capacity;
	unordered_map<TileID, Target, TileIDHasher> _cache;

	unique_ptr<Target> _tile_read_only;

private:
	void SwapOut();
	void SetMaxCapacity(uint max_capacity);

public:
	const Size GetTileSize() const { return _tile_size; }
	void ResetTileSize(Size layer_size);

	const Rect GetCachedRegion() const { return _cached_region; }
	void SetCachedRegion(Rect accessible_region, Rect visible_region);

	const Target& ReadTile(TileID tile_id) const;
	Target& WriteTile(TileID tile_id);


	///////////////////////////////////////////////////////////
	////                      Drawing                      ////
	///////////////////////////////////////////////////////////
public:
	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region);
};


struct Background;

struct LayerFigure : Figure {
	const Layer& layer;
	const Background& background;
	Rect region;
	CompositeEffect composite_effect;

	LayerFigure(const Layer& layer, const Background& background, Rect region, CompositeEffect composite_effect) :
		layer(layer), background(background), region(region), composite_effect(composite_effect) {
	}
	virtual const Rect GetRegion() const override { return Rect(point_zero, region.size); }
	virtual void DrawOn(RenderTarget& target, Vector offset) const override;  // defined in figure_types.cpp
};


END_NAMESPACE(WndDesign)