#pragma once

#include "span.h"

namespace valkyr {
	//store in each chunk used by a container, providing info for the chunk's elements 
	//and for fast visiting elements cross chunks
	//a proxy of a container in each chunk
	template <typename T>
	struct ContainerChunkNode {
		Chunk* prev;
		Chunk* next;
		Span<T>* firstSpan;
		unsigned int spanCount;
	};
}
