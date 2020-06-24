#pragma once
#include <vcruntime_string.h>
#include <xmemory>

namespace valkyr {
	const int MAX_CHUNK_SIZE = 16*1024*1024;

	union Chunk {
		char buff[MAX_CHUNK_SIZE]; //= new char[MAX_CHUNK_SIZE-sizeof(int)];
		Chunk* next; //= nullptr;
	};

	class ChunkAllocator {
	public:
		static inline Chunk* Malloc() {
			Chunk* chunk = (Chunk*)malloc(MAX_CHUNK_SIZE);
			memset(chunk->buff, 0, sizeof(chunk));
			chunk->next = nullptr;
			return chunk;
		}

	};

	class Pool {
		Chunk* mCurrChunk;
		Chunk* mFirstChunk;
		size_t mChunkSize;

		void NewChunk() {
			Chunk* chunk = ChunkAllocator::Malloc();
			if (mCurrChunk == nullptr) {
				mCurrChunk = chunk;
			}
			if (mFirstChunk == nullptr) {
				mFirstChunk = mCurrChunk;
			}
		}
	};

};