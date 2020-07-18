#pragma once
#ifdef WIN32
#include <vcruntime_string.h>
#endif
#include <xmemory>
#include <functional>

namespace valkyr {
	const unsigned int CHUNK_SIZE = 16 * 1024 * 1024;
	const unsigned int POOL_CHUNKS = 4;

	struct Chunk {
		char buff[CHUNK_SIZE];
		//Chunk* next;
	};

	struct ChunkInfo {
		//alignas(4) bool isFull;
		Chunk* next;
		int head;
		size_t usedSize;
		size_t unUsedSize;

		ChunkInfo() {
			//isFull = false;
			next = nullptr;
			head = sizeof(ChunkInfo);
			usedSize = sizeof(ChunkInfo);
			unUsedSize = CHUNK_SIZE - sizeof(ChunkInfo);
		}
	};

	class ChunkAllocator {
	public:
		static inline Chunk* Malloc() {
			Chunk* chunk = (Chunk*)malloc(CHUNK_SIZE);
			memset(chunk->buff, 0, sizeof(chunk));
			ChunkInfo* info = new(chunk->buff) ChunkInfo();
			return chunk;
		}

		static inline void Free(Chunk* chunk) {
			//chunk->next = nullptr;
			free(chunk->buff);
		}
	};

	class ChunkUtil {
	public:
		static inline ChunkInfo* GetInfo(Chunk* chunk) {
			if (chunk == nullptr) return nullptr;
			return (ChunkInfo*)chunk->buff;
		}

		static inline Chunk* GetNext(Chunk* chunk) {
			if (chunk == nullptr) return nullptr;
			return GetInfo(chunk)->next;
		}

		static inline bool IsFull(Chunk* chunk) {
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return info->unUsedSize == 0 ;
		}

		template <typename T>
		static inline bool CanContain(Chunk* chunk) {
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return info->unUsedSize >= sizeof(T);
		}

		static inline void Connect(Chunk* chunk,Chunk* next) {
			ChunkUtil::GetInfo(chunk)->next = next;
		}

		template <typename T>
		static inline T* NewObjFrom(Chunk* chunk) {
			ChunkInfo* info = GetInfo(chunk);
			if (info->unUsedSize < sizeof(T)) {
				return nullptr;
			}
			T* obj = new (chunk->buff + info->head)T();
			info->usedSize += sizeof(T);
			if (info->usedSize >= CHUNK_SIZE) {
				info->usedSize = CHUNK_SIZE;
				info->unUsedSize = 0;
				info->head = CHUNK_SIZE;
			}
			else {
				info->head += sizeof(T);
				info->usedSize += sizeof(T);
				info->unUsedSize -= sizeof(T);
			}
			return obj;
		}

		template <typename T>
		static inline T* GetFrom(int idx, Chunk* chunk) {
			return (T*)(chunk->buff + idx);
		}
	};

	struct Arena
	{
		Chunk* lastChunk;
		Chunk* firstChunk;
		size_t chunkCount;
		int pad;
	};

	class ArenaUtil {
	public:
		static inline Arena* Create() {
			Chunk* chunks[POOL_CHUNKS];
			for (int i = 0; i < POOL_CHUNKS; ++i) {
				chunks[i] = ChunkAllocator::Malloc();
				if (i > 0) {
					/*chunks[i - 1]->next = chunks[i];*/
					ChunkUtil::Connect(chunks[i - 1],chunks[i]);
				}
			}
			Arena* arena = ChunkUtil::NewObjFrom<Arena>(chunks[0]);
			arena->chunkCount = POOL_CHUNKS;
			arena->firstChunk = chunks[0];
			arena->lastChunk = chunks[POOL_CHUNKS-1];
			return arena;
		}

		template <typename T>
		static inline T* NewObjFrom(Arena* arena) {
			Chunk* curr = arena->firstChunk;
			T* obj = nullptr;
			while (curr!=nullptr) {
				ChunkInfo* info = ChunkUtil::GetInfo(curr);
				Chunk* next = info->next;
				obj = ChunkUtil::NewObjFrom<T>(curr);
				if (obj != nullptr) {
					return obj;
				}
				curr = next;
			}
			Chunk* chunk = ChunkAllocator::Malloc();
			ChunkUtil::GetInfo(curr)->next = chunk;
			arena->lastChunk = chunk;
			arena->chunkCount++;
			obj = ChunkUtil::NewObjFrom<T>(chunk);
			return obj;
		}

		static inline void Traverse(Arena* arena, std::function<void(Chunk*,int)> action) {
			Chunk* curr = arena->firstChunk;
			for (int i = 0; i < arena->chunkCount; i++) {
				if (curr == nullptr) return;
				Chunk* next = ChunkUtil::GetNext(curr);
				if(action)
					action(curr,i);
				if (next == nullptr) return;
				curr = next;
			}
		}
		

		static inline void Clear(Arena* arena) {
			arena->lastChunk = nullptr;
			Traverse(arena,[arena](Chunk* chunk,int i){
				if (arena->firstChunk != chunk) {
					ChunkAllocator::Free(chunk);
				}
			});
			ChunkAllocator::Free(arena->firstChunk);
			arena = nullptr;
		}

	};
};