#include <memory>

using std::unique_ptr;

template<class T>
using ref_ptr = T*;

template<class T>
using alloc_ptr = T*;


struct Vector { int x; int y; };

class _Figure_List_Node {
private:
	friend class FigureQueue;
	friend class _Figure_Iter;
	mutable alloc_ptr<const _Figure_List_Node> next;
	mutable Vector offset;

public:
	_Figure_List_Node() : next(nullptr), offset() {}
	const Vector Offset() const { return offset; }
};


struct Figure : public _Figure_List_Node {
	virtual void DrawOn() const = 0;
};

struct Figure1 : public Figure {
	mutable int a;
	virtual void DrawOn() const override { a = 1; }
};

struct Figure2 : public Figure {
	mutable int a, b, c, d;
	virtual void DrawOn() const override { a = b = c = d = 2; }
};


class _Figure_Iter {
private:
	ref_ptr<const Figure> node;
public:
	_Figure_Iter(const Figure* node) : node(node) {}
	bool operator!=(const _Figure_Iter& iter) { return node != iter.node; }
	void operator++() { node = static_cast<const Figure*>(node->next); }
	const Figure& operator*() { return *node; }
};


class FigureQueue {
private:
	_Figure_List_Node sentinel;
	ref_ptr<const _Figure_List_Node> _head;
	alloc_ptr<const Figure> _tail;

public:
	FigureQueue() : _head(&sentinel), _tail(static_cast<Figure*>(&sentinel)) {}
	~FigureQueue() { Clear(); }

	_Figure_Iter begin() const { return { static_cast<const Figure*>(_head->next) }; }
	_Figure_Iter end() const { return { static_cast<const Figure*>(_tail->next) }; }

	void Append(Vector offset, alloc_ptr<const Figure> figure) {
		figure->offset = offset;
		_tail->next = figure;
		_tail = figure;
	}

	bool IsEmpty() const { return _head == _tail; }

	void Clear() {
		const Figure* curr = static_cast<const Figure*>(_head->next);
		while (curr) {
			const Figure* temp = static_cast<const Figure*>(curr->next);
			delete curr;
			curr = temp;
		}
		_head->next = nullptr;
		_tail = static_cast<const Figure*>(_head);
	}
};


int main() {
	FigureQueue figure_queue;
	figure_queue.Append({ 1,1 }, new Figure1);
	figure_queue.Append({ 2,2 }, new Figure2);
	figure_queue.Append({ 3,3 }, new Figure1);

	const FigureQueue& const_figure_queue = figure_queue;
	for (auto& it : const_figure_queue) {
		Vector vector = it.Offset();
		it.DrawOn();
	}
}