#pragma once

#include "core.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;


template<class T>
struct list_iterator : list<T>::iterator {
	list_iterator(typename list<T>::iterator it = {}) : list<T>::iterator(it) {}
	bool valid() const { return this->_Ptr != nullptr; }
};


END_NAMESPACE(WndDesign)