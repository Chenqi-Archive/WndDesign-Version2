#include "style_helper.h"


BEGIN_NAMESPACE(WndDesign)

static const char style_parse_exception[] = "style parse error";


inline void BoundLengthBetween(ValueTag& normal_length, ValueTag min_length, ValueTag max_length) {
	if (!min_length.IsAuto() && normal_length.AsUnsigned() < min_length.AsUnsigned()) { normal_length.Set(min_length.AsUnsigned()); }
	if (!max_length.IsAuto() && normal_length.AsUnsigned() > max_length.AsUnsigned()) { normal_length.Set(max_length.AsUnsigned()); }
}

const Interval CalculateLengthFromStyle(ValueTag normal_length, ValueTag min_length, ValueTag max_length, ValueTag position_low, ValueTag position_high, uint parent_length) {
	if (parent_length == 0) { return Interval(0, 0); }

	normal_length.ConvertToPixel(parent_length);
	min_length.ConvertToPixel(parent_length);
	max_length.ConvertToPixel(parent_length);
	position_low.ConvertToPixel(parent_length);
	position_high.ConvertToPixel(parent_length);

	if (normal_length.IsAuto()) {
		if (position_low.IsAuto() || position_low.IsCenter() || position_high.IsAuto() || position_high.IsCenter()) {
			throw std::invalid_argument(style_parse_exception);
			normal_length.Set(parent_length);
		} else {
			normal_length.Set(position_high.AsSigned() - position_low.AsSigned());
		}
	}
	BoundLengthBetween(normal_length, min_length, max_length);

	if (position_low.IsAuto() || position_low.IsCenter()) {
		if (position_low.IsCenter()) {
			position_low.Set((static_cast<int>(parent_length) - normal_length.AsSigned()) / 2);
		} else if (!position_high.IsAuto()) {
			position_low.Set(static_cast<int>(parent_length) - position_high.AsSigned() - normal_length.AsSigned());
		} else {
			throw std::invalid_argument(style_parse_exception);
			position_low.Set(0);
		}
	}

	return Interval(position_low.AsSigned(), normal_length.AsUnsigned());
}


END_NAMESPACE(WndDesign)