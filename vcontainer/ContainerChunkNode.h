#pragma once

#include "span.h"

namespace valkyr {
	//store in each chunk used by a container, providing info for the chunk's elements 
	//and for fast visiting elements in a specific chunk
	//a proxy of a container in each chunk
	template <typename T>
	struct ContainerChunkNode {
		ContainerChunkNode* prev;
		ContainerChunkNode* next;
		ChunkInfo* chunkInfo;
		Chunk* chunk;
		Span<T>* firstSpan;
		unsigned int spanCount;
		int pad[2];
	};
}
