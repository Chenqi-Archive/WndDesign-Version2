#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <memory>
#include <assert.h>

using namespace std;

using uint = unsigned;

struct Interval {
	int begin;
	uint length;

	explicit constexpr Interval(int begin, uint length) : begin(begin), length(length) {}
	explicit constexpr Interval(uint begin, uint length) : begin(static_cast<int>(begin)), length(length) {}

	int left() const { return begin; }
	int right() const { return begin + static_cast<int>(length); }

	bool IsEmpty() const { return length == 0; }

	bool operator==(const Interval& interval) const { return begin == interval.begin && length == interval.length; }
	bool operator!=(const Interval& interval) const { return begin != interval.begin || length != interval.length; }

	bool Contains(int number) const { return number >= left() && number < right(); }
	bool Contains(const Interval& interval) const { return left() >= interval.left() && right() <= interval.right(); }

	const Interval Intersect(const Interval& interval) const {
		int left_max = max(left(), interval.left()), right_min = min(right(), interval.right());
		if (right_min > left_max) {
			return Interval(left_max, static_cast<uint>(right_min - left_max));
		} else {
			return Interval(left_max, 0);
		}
	}

	const Interval Union(const Interval& interval) const {
		if (IsEmpty()) {
			return interval;
		} else if (interval.IsEmpty()) {
			return *this;
		} else {
			int left_min = min(left(), interval.left()), right_max = max(right(), interval.right());
			return Interval(left_min, static_cast<uint>(right_max - left_min));
		}
	}
};


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


class Style {
	int style;
public:
	Style(int style) : style(style) {}
	bool Equals(const Style& style) const { return this->style == style.style; }
	unique_ptr<Style> Clone() const { return make_unique<Style>(style); }
	int Get() const { return style; }
};

static const inline Style style_null = 0;



class IntervalStyleList {
private:
	struct IntervalStyle {
		Interval interval;
		unique_ptr<Style> style;
	};
	list<IntervalStyle> styles;

public:
	list<IntervalStyle>::iterator ClearStyle(Interval interval) {
		auto it = styles.begin();
		while(it != styles.end()) {
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
					if(it_prev->interval.length == 0) {
						it_prev->interval.begin = interval.right(); it_prev->interval.length = length;
						it = it_prev;
					} else if (length != 0) {
						styles.emplace(it, IntervalStyle{ Interval(interval.right(), length), it_prev->style->Clone() });
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
	void SetStyle(const Style& style, Interval interval) {
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
			styles.emplace(it, IntervalStyle{ interval, style.Clone() });
		}
	}
	void ExtendStyle(Interval interval) {
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
	void ShrinkStyle(Interval interval) {
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

	void Print() {
		for (auto& style : styles) {
			cout << "    [" << style.interval.left() << "," << style.interval.right() << "): " << style.style->Get() << "\n";
		}
	}
};

int main() {
	IntervalStyleList styles;
	while (1) {
		int type;  // 0,1,2,3
		cin >> type;
		int style;
		uint begin, length;
		if (type == 0) {
			cin >> style;
		}
		cin >> begin >> length;

		if (length == 0) { continue; }  // 

		switch (type) {
		case 0: 
			styles.SetStyle(style, Interval(begin, length));
			break;
		case 1:
			styles.ClearStyle(Interval(begin, length));
			break;
		case 2:
			styles.ExtendStyle(Interval(begin, length));
			break;
		case 3:
			styles.ShrinkStyle(Interval(begin, length));
		default:
			break;
		}
		styles.Print();
	}
}