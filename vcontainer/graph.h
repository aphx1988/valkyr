#pragma once
#include "vec.h"
//#include "../vmalloc/ptr.h"

namespace valkyr {
	template <class T>
	struct Node {
		T data;
		size_t type;
		SmallVec<Node<T>> inputs;
		SmallVec<Node<T>> outputs;
	};

	template <class NODE>
	class Graph {
	public:
		virtual void Execute() = 0;
		SmallVec<NODE> m_rootLayer;
	};
}
