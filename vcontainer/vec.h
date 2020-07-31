#pragma once

#include "ContainerChunkNode.h"
#include "span.h"
#include "../vmalloc/ChunkMgr.h"

namespace valkyr {
	template <typename T>
	struct Vec {
		unsigned int length;
		unsigned int chunkCount;
		ContainerChunkNode<T>* firstChunkNode;
		ContainerChunkNode<T>* lastChunkNode;
		Chunk* createdChunk;
		int pad[3];
	};

	class VecUtil {
	public:
		template <typename T>
		static inline Vec<T>* Create(ChunkMgr* chunkMgr,unsigned int num) {
			//int numNeededChunks = num * size(T);
			Vec<T>* list = nullptr;
			Chunk* chunk = nullptr;
			if (ChunkUtil::CanContain(chunkMgr->m_currChunk,num)) {
				chunk = chunkMgr->m_currChunk;
			}
			//it's rare that the new created vec using more than one chunks,so here need just one chunk
			else {
				chunk = chunkMgr->UseNextChunk(nullptr);
			}
			list = ChunkUtil::NewObjFrom<Vec<T>>(chunk);
			ContainerChunkNode<T>* chunkNode = ChunkUtil::NewObjFrom<ContainerChunkNode<T>>(chunk);
			chunkNode->firstSpan = SpanUtil::Create(chunk, num);
			list->length = num;
			list->chunkCount = 1;
			list->createdChunk = chunk;
			list->firstChunkNode = chunkNode;
			list->lastChunkNode = chunkNode;
			return list;
		}

		template <typename T>
		static inline void PushTo(T&& t, Vec<T>* list) {

		}
	};
}