#include "text_block.h"
#include "../system/directx/directx_helper.h"
#include "../system/directx/dwrite_api.h"


BEGIN_NAMESPACE(WndDesign)


enum class IntervalRelation {
	Left,
	LeftIntersect,
	Overlapped,
	Overlapping,
	RightIntersect,
	Right
};

// a is #relation# relative to b.
inline IntervalRelation TestIntervalRelation(Interval a, Interval b) {
	assert(!a.IsEmpty() && !b.IsEmpty());
	if (a.right() <= b.left()) { return IntervalRelation::Left; }
	if (a.right() < b.right() && a.left() < b.left()) { return IntervalRelation::LeftIntersect; }
	if (a.right() <= b.right() && a.left() >= b.left()) { return IntervalRelation::Overlapped; }
	if (a.right() >= b.right() && a.left() <= b.left()) { return IntervalRelation::Overlapping; }
	if (a.right() > b.right() && a.left() > b.left()) { return IntervalRelation::RightIntersect; }
	if (a.left() >= b.right()) { return IntervalRelation::Right; }
	assert(0); return IntervalRelation::Right;
}

list<TextBlock::TextStyleIntervalList::TextStyleInterval>::iterator 
TextBlock::TextStyleIntervalList::ClearStyle(Interval interval) {
	auto it = styles.begin();
	while (it != styles.end()) {
		switch (TestIntervalRelation(it->interval, interval)) {
		case IntervalRelation::Left:
			it++; break;
		case IntervalRelation::LeftIntersect:
			it->interval.length = interval.begin - it->interval.begin; it++; break;
		case IntervalRelation::Overlapped:
			styles.erase(it++); break;
		case IntervalRelation::Overlapping: {
				uint length = static_cast<uint>(it->interval.right() - interval.right());
				auto it_prev = it++;
				it_prev->interval.length = interval.begin - it_prev->interval.begin;
				if (it_prev->interval.length == 0) {
					it_prev->interval.begin = interval.right(); it_prev->interval.length = length;
					it = it_prev;
				} else if (length != 0) {
					styles.emplace(it, TextStyleInterval{ Interval(interval.right(), length), it_prev->style->Clone() });
					it--;
				}
			}return it;
		case IntervalRelation::RightIntersect:
			it->interval.length = static_cast<uint>(it->interval.right() - interval.right());
			it->interval.begin = interval.right();
			return it;
		case IntervalRelation::Right:
			return it;
		default: assert(0);
		}
	}
	return it;
}

void TextBlock::TextStyleIntervalList::SetStyle(Interval interval, const TextStyleBase& style) {
	auto it = ClearStyle(interval);
	auto it_prev = it;
	bool extended = false;
	if (it != styles.begin()) {
		it_prev--;
		if (it_prev->interval.right() == interval.left() && it_prev->style->Equals(style)) {
			it_prev->interval.length += interval.length;
			extended = true;
		}
	}
	if (it != styles.end()) {
		if (interval.right() == it->interval.left() && it->style->Equals(style)) {
			if (extended) {
				it_prev->interval.length += it->interval.length;
				styles.erase(it++);
			} else {
				it->interval.length += interval.length;
				it->interval.begin = interval.begin;
				extended = true;
			}
		}
	}
	if (!extended) {
		styles.emplace(it, TextStyleInterval{ interval, style.Clone() });
	}
}

void TextBlock::TextStyleIntervalList::ExtendStyle(Interval interval) {
	auto it = styles.begin();
	while (it != styles.end()) {
		if (it->interval.right() < interval.left()) { it++; continue; }
		if (it->interval.Contains(interval.begin - 1)
			|| (it->interval.begin == interval.begin && interval.begin == 0)) {
			it->interval.length += interval.length;
			it++; break;
		}
		if (it->interval.left() >= interval.left()) {
			break;
		}
	}
	while (it != styles.end()) {
		it->interval.begin += static_cast<int>(interval.length);
		it++;
	}
}

void TextBlock::TextStyleIntervalList::ShrinkStyle(Interval interval) {
	auto it = ClearStyle(interval);
	auto it_prev = it;
	for (auto it_right = it; it_right != styles.end(); ++it_right) {
		it_right->interval.begin -= static_cast<int>(interval.length);
	}
	if (it != styles.begin()) {
		it_prev--;
		if (it_prev->interval.right() == it->interval.left() && it_prev->style->Equals(*it->style)) {
			it_prev->interval.length += it->interval.length;
			styles.erase(it);
		}
	}
}

void TextBlock::TextStyleIntervalList::ApplyTo(TextLayout& layout) const {
	for (auto& style : styles) { style.style->ApplyTo(layout, style.interval); }
}


inline void TextBlock::_SetStyle(uint begin, uint length, const TextStyleBase& style) {
	styles[style.GetTypeID()].SetStyle(Interval(begin, length), style);
}

inline void TextBlock::_ClearStyle(uint begin, uint length) {
	for (auto& style : styles) { style.ClearStyle(Interval(begin, length)); }
}

inline void TextBlock::_ExtendStyle(uint begin, uint length) {
	for (auto& style : styles) { style.ExtendStyle(Interval(begin, length)); }
}

inline void TextBlock::_ShrinkStyle(uint begin, uint length) {
	for (auto& style : styles) { style.ShrinkStyle(Interval(begin, length)); }
}

inline void TextBlock::_ApplyAllStyles() {
	for (auto& style : styles) { style.ApplyTo(static_cast<TextLayout&>(layout->DWriteTextLayout())); }
}


void TextBlock::SetStyle(uint begin, uint length, const TextStyleBase& style) {
	_SetStyle(begin, length, style);
	style.ApplyTo(static_cast<TextLayout&>(layout->DWriteTextLayout()), Interval(begin, length));
}

void TextBlock::ClearStyle(uint begin, uint length) {
	_ClearStyle(begin, length);

	// Simply recreate the layout.
	layout->TextChanged(text);
	_ApplyAllStyles();
}

void TextBlock::TextInsertedResetStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles) {
	_ExtendStyle(begin, length);
	_ClearStyle(begin, length);
	for (auto& style : styles) { _SetStyle(begin, length, *style); }

	layout->TextChanged(text);
	_ApplyAllStyles();
}

void TextBlock::TextInsertedMergeStyle(uint begin, uint length, const vector<unique_ptr<TextStyleBase>>& styles) {
	_ExtendStyle(begin, length);
	for (auto& style : styles) { _SetStyle(begin, length, *style); }

	layout->TextChanged(text);
	_ApplyAllStyles();
}

void TextBlock::TextInsertedWithoutStyle(uint begin, uint length) {
	_ExtendStyle(begin, length);

	layout->TextChanged(text);
	_ApplyAllStyles();
}

void TextBlock::TextDeleted(uint begin, uint length) {
	_ShrinkStyle(begin, length);

	layout->TextChanged(text);
	_ApplyAllStyles();
}


END_NAMESPACE(WndDesign)