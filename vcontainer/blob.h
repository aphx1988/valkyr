#pragma once

#include "../vmalloc/ChunkMgr.h"

namespace valkyr {
	struct Part {
		Part* prev;
		Part* next;
		Chunk* chunk;
 		unsigned int size;
	};

	struct Blob {
		Part* firstPart;
		Part* lastPart;
		Chunk* createdChunk;
		ChunkMgr* chunkMgr;
		unsigned int numParts;
		unsigned int chunkCount;
		int pad[2] = { 0.0 };
	};

	class BlobUtil {
	public:
		/*static bool CanCreate(Chunk* chunk) {
			auto* info = ChunkUtil::GetInfo(chunk);
			bool r = CHUNK_SIZE - info->usedSize >= sizeof(Blob)+ sizeof(Part);
			if (!r) {
				r = CHUNK_SIZE - info->usedSize >= sizeof(T)
					+ sizeof(ContainerChunkNode<T>) + sizeof(Pool<T>);
			}
			return r;
		}*/

		static Blob* Create(ChunkMgr* chunkMgr) {
			Blob* blob = nullptr;
			Chunk* chunk = nullptr;

		}
	};
}
