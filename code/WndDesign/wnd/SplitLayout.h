#pragma once

#include "Wnd.h"


BEGIN_NAMESPACE(WndDesign)


class SplitLayout : public Wnd {
public:
	using Style = Wnd::Style;

	struct NodeStyle {
	public:
		uint _line_width = 3;
		Color _line_color = ColorSet::Black;
		enum : ushort { Vertical, Horizonal } _direction = Vertical;
		enum : ushort { First, Second } _which = First;
		ValueTag _position = 40pct;
	public:
		constexpr NodeStyle& line_width(ValueTag line_width) {
			if (!line_width.IsPixel()) { throw std::invalid_argument("line width can only be in pixel"); }
			_line_width = line_width.AsUnsigned(); return *this;
		}
		constexpr NodeStyle& line_color(Color line_color) { _line_color = line_color; return *this; }
		constexpr NodeStyle& direction(bool is_horizontal) { _direction = is_horizontal ? Horizonal : Vertical; return *this; }
		constexpr NodeStyle& which(bool is_second) { _which = is_second ? Second : First; return *this; }
		constexpr NodeStyle& position(ValueTag position) { _position = position; return *this; }
	};

public:
	SplitLayout(unique_ptr<Style> style) : Wnd(std::move(style)) {}
	~SplitLayout() {}

public:
	class Node {
	private:
		ref_ptr<Node> parent;
		Rect region;
		union {
			struct { // as internal node
				alloc_ptr<Node> first, second;
				NodeStyle style;
			};
			struct { // as leaf node
				ref_ptr<WndObject> wnd;
			};
		};
	protected:
		friend class SplitLayout;
		Node() : parent(nullptr), region(region_empty), first(nullptr), second(nullptr), style() {}
		~Node() {
			if (IsLeaf()) {
				if (wnd != nullptr) {
					wnd->GetParent()->RemoveChild(*wnd);
				}
			} else {
				delete first; //first = nullptr;
				delete second; //second = nullptr;
			}
		}
		void SetWnd(WndObject& wnd) {
			if (!IsLeaf()) {
				delete first; first = nullptr;
				delete second; second = nullptr;
			}
			this->wnd = &wnd;
		}
		ref_ptr<WndObject> GetWnd() const { assert(IsLeaf()); return wnd; }
	public:
		bool IsLeaf() const { return second == nullptr; }
		void Split(NodeStyle style, bool is_second = false) {
			alloc_ptr<Node> first = new Node();
			alloc_ptr<Node> second = new Node();
			is_second ? *second = *this : *first = *this;
		#error child node reference changed. parent node pointer?
			this->first = first; this->second = second;
			this->style = style;
		}
		Node& GetFirst() const {
			if (IsLeaf()) { throw std::invalid_argument("node is not an internal node"); }
			return *first;
		}
		Node& GetSecond() const {
			if (IsLeaf()) { throw std::invalid_argument("node is not an internal node"); }
			return *second;
		}
	};
private:
	Node _root;
public:
	Node& GetRootNode() { return _root; }
	const Node& GetRootNode() const { return _root; }


	//// child windows ////

	void SetChild(Node& node, WndObject& child) {

	}

private:
	struct HitTestInfo {
		ref_ptr<const Node> node;
		Point point;
	};

	// The point must fall on a leaf node, or the split line of an internal node.
	const HitTestInfo HitTestPoint(Point point) const {
		ref_ptr<const Node> node = &GetRootNode();
		if (!node->region.Contains(point)) { return { nullptr, point }; }
		while (true) {
			if (!node->IsLeaf()) {
				const Node& first = node->GetFirst();
			#error Is point absolute or relative?
				if (first.region.Contains(point)) { node = &first; continue; }
				const Node& second = node->GetSecond();
				if (second.region.Contains(point)) { node = &second; continue; }
			}
			return HitTestInfo{ node, point };
		}
		assert(false);
	}

protected:
	virtual void Handler(Msg msg, Para para) override { return true; }
};


END_NAMESPACE(WndDesign)