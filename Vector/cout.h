#pragma once
#include "Vector.h"

template<typename T>
std::ostream& operator<<(std::ostream& os, Vector<T>& v) {
	for (int i = 0; i < v.size(); ++i) {
		os << v[i];
	}

	return os;
}