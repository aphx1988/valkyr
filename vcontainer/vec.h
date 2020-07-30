#pragma once

#include "ContainerChunkNode.h"

namespace valkyr {
	template <typename T>
	struct Vec {
		unsigned int length;
		unsigned int m_chunkCount;
		ContainerChunkNode<T>* firstChunkNode;
		ContainerChunkNode<T>* currChunkNode;
		Chunk* createdChunk;
		int pad[3];
	};

	class VecUtil {
	public:
		template <typename T>
		static inline Vec<T>* Create(Chunk* chunk,unsigned int num) {
			Vec<T>* list = ChunkUtil::NewObjFrom<Vec<T>>(chunk);
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create(chunk, num);
			list->length = 0;
			list->m_chunkCount = 0;
		}

		template <typename T>
		static inline void PushTo(T&& t, Vec<T>* list) {

		}
	};
}