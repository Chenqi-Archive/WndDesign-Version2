#pragma once

#include "../geometry/geometry.h"


BEGIN_NAMESPACE(WndDesign)


// The max size of a tile when region size does not change, if changed, use the variable sizes below.
constexpr Size tile_size_max_fixed = Size(1024, 1024);
// Used as the initial previous_tile_size.
constexpr Size tile_size_empty = Size(32, 32);

// The variable tile sizes.
constexpr uint length_round_up = 32;
constexpr Size tile_size_min = Size(64, 64);
constexpr Size tile_size_max = Size(384, 384);


inline uint RoundUpToMultipleOf(uint length, uint round) {
	uint remainder = length % round;
	if (remainder > 0) { return length + round - remainder; } else { return length; }
}

inline const Size CalculateTileSize(Size entire_size, Size previous_tile_size) {
	Size tile_size;
	// For empty region, return the initial size.
	if (entire_size.IsEmpty()) { return previous_tile_size; }
	// For the first time to calculate, assume the size will be fixed.
	if (previous_tile_size == tile_size_empty) {
		// Round up the length.
		tile_size.width = RoundUpToMultipleOf(entire_size.width, length_round_up);
		tile_size.height = RoundUpToMultipleOf(entire_size.height, length_round_up);
		// If the length is larger than the max fixed tile length, use variable tile size.
		if (tile_size.width > tile_size_max_fixed.width) { tile_size.width = tile_size_max.width; }
		if (tile_size.height > tile_size_max_fixed.height) { tile_size.height = tile_size_max.height; }
		// The length should be greater than the min tile length.
		if (tile_size.width < tile_size_min.width) { tile_size.width = tile_size_min.width; }
		if (tile_size.height < tile_size_min.height) { tile_size.height = tile_size_min.height; }
		return tile_size;
	}
	// The tile size need to be recalculated, the calculation below for width and height is independent.
	// For width:
	if (previous_tile_size.width == RoundUpToMultipleOf(entire_size.width, length_round_up)) {
		// If the entire region has not changed, use the previous tile size:
		tile_size.width = previous_tile_size.width;
	} else {
		// The entire region has changed, use variable tile sizes:
		if (previous_tile_size.width >= tile_size_max.width) {
			// If previous tile size is larger than the max variable tile size, use the max tile size.
			tile_size.width = tile_size_max.width;
		} else {
			if (entire_size.width <= previous_tile_size.width * 3) {
				// If previous tile size multiplied by 3 is bigger than the region size, use the previous size.
				tile_size.width = previous_tile_size.width;
			} else {
				// Else, enlarge the tile size. The new length must be greater than pervious tile length.
				tile_size.width = RoundUpToMultipleOf(entire_size.width, length_round_up * 2) / 2;
				// If the length is larger than the max tile length, use max tile size.
				if (tile_size.width > tile_size_max.width) { tile_size.width = tile_size_max.width; }
			}
		}
	}
	// For height: use the same rule.
	if (previous_tile_size.height == RoundUpToMultipleOf(entire_size.height, length_round_up)) {
		tile_size.height = previous_tile_size.height;
	} else {
		if (previous_tile_size.height >= tile_size_max.height) {
			tile_size.height = tile_size_max.height;
		} else {
			if (entire_size.height <= previous_tile_size.height * 3) {
				tile_size.height = previous_tile_size.height;
			} else {
				tile_size.height = RoundUpToMultipleOf(entire_size.height, length_round_up * 2) / 2;
				if (tile_size.height > tile_size_max.height) { tile_size.height = tile_size_max.height; }
			}
		}
	}
	return tile_size;
}


// The vector start from the origin point, and re
inline const Vector ClipVectorInsideRectangle(int half_width, int half_height, Vector vector) {
	float shrink_ratio = 1;
	assert(half_width >= 0 && half_height >= 0);
	if (vector.x > half_width) { shrink_ratio = min(shrink_ratio, half_width / (float)vector.x); }
	if (vector.x < -half_width) { shrink_ratio = min(shrink_ratio, -half_width / (float)vector.x); }
	if (vector.y > half_height) { shrink_ratio = min(shrink_ratio, half_height / (float)vector.y); }
	if (vector.y < -half_height) { shrink_ratio = min(shrink_ratio, -half_height / (float)vector.y); }
	return Vector(static_cast<int>(vector.x * shrink_ratio), static_cast<int>(vector.y * shrink_ratio));
}

// Scale the region by length in ratio(> 1). And shift the larger region by offset_hint.
inline const Rect ScaleRegion(const Rect& region, float ratio, Vector offset_hint = vector_zero) {
	if (ratio <= 1.0) { return region; }
	Size size = region.size;
	size.width = static_cast<uint>(static_cast<float>(size.width) * ratio);
	size.height = static_cast<uint>(static_cast<float>(size.height) * ratio);
	int width_padding = static_cast<int>(size.width - region.size.width) / 2;
	int height_padding = static_cast<int>(size.height - region.size.height) / 2;
	offset_hint = ClipVectorInsideRectangle(width_padding, height_padding, offset_hint);
	Point point = region.point;
	point.x = point.x - width_padding + offset_hint.x;
	point.y = point.y - height_padding + offset_hint.y;
	return Rect(point, size);
}

inline const Rect EnlargeRegion(Rect region, uint enlarge_length, Vector offset_hint = vector_zero) {
	offset_hint = ClipVectorInsideRectangle(enlarge_length, enlarge_length, offset_hint);
	region.point.x = region.point.x - static_cast<int>(enlarge_length) + offset_hint.x;
	region.point.y = region.point.y - static_cast<int>(enlarge_length) + offset_hint.x;
	region.size.width += 2 * enlarge_length;
	region.size.height += 2 * enlarge_length;
	return region;
}


END_NAMESPACE(WndDesign)