#pragma once
#include <xmemory>

namespace valkyr {
	const unsigned int CHUNK_SIZE = 16 * 1024 * 1024;

	struct Chunk {
		char buff[CHUNK_SIZE];
		//Chunk* next;
	};

	struct ChunkInfo {
		//alignas(4) bool isFull;
		Chunk* next;
		Chunk* prev;
		unsigned int head;
		unsigned int usedSize;

		ChunkInfo() {
			//isFull = false;
			prev = nullptr;
			next = nullptr;
			head = sizeof(ChunkInfo);
			usedSize = sizeof(ChunkInfo);
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

		static inline Chunk* GetPrev(Chunk* chunk) {
			if (chunk == nullptr) return nullptr;
			return GetInfo(chunk)->prev;
		}

		static inline bool IsFull(Chunk* chunk) {
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return info->usedSize == CHUNK_SIZE;
		}

		static inline bool IsFull(ChunkInfo* info) {
			return info->usedSize == CHUNK_SIZE;
		}

		template <typename T>
		static inline bool CanContain(Chunk* chunk) {
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= sizeof(T);
		}

		template <typename T>
		static inline bool CanContain(Chunk* chunk,size_t num) {
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= sizeof(T)*num;
		}

		template <typename T>
		static inline size_t GetMaxCount(Chunk* chunk) {
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return (CHUNK_SIZE - info->usedSize) / sizeof(T);
		}

		static inline void Connect(Chunk* chunk, Chunk* next) {
			ChunkUtil::GetInfo(chunk)->next = next;
			ChunkUtil::GetInfo(next)->prev = chunk;
		}

		template <typename T>
		static inline T* NewObjFrom(Chunk* chunk) {
			ChunkInfo* info = GetInfo(chunk);
			if (CHUNK_SIZE - info->usedSize < sizeof(T)) {
				return nullptr;
			}
			T* obj = new (chunk->buff + info->head)T();
			info->usedSize += sizeof(T);
			if (info->usedSize >= CHUNK_SIZE) {
				info->usedSize = CHUNK_SIZE;
				info->head = CHUNK_SIZE;
			}
			else {
				info->head += sizeof(T);
				info->usedSize += sizeof(T);
			}
			return obj;
		}

		template <typename T, typename ...Args>
		static inline T* NewObjFrom(Chunk* chunk, Args... args) {
			ChunkInfo* info = GetInfo(chunk);
			if (CHUNK_SIZE - info->usedSize < sizeof(T)) {
				return nullptr;
			}
			T* obj = new (chunk->buff + info->head)T(std::forward<Args>(args)...);
			info->usedSize += sizeof(T);
			if (info->usedSize >= CHUNK_SIZE) {
				info->usedSize = CHUNK_SIZE;
				info->head = CHUNK_SIZE;
			}
			else {
				info->head += sizeof(T);
				info->usedSize += sizeof(T);
			}
			return obj;
		}


		template <typename T>
		static inline T* Get(unsigned int idx, Chunk* chunk) {
			return reinterpret_cast<T*>(chunk->buff + idx);
		}

		static inline void Zero(Chunk* chunk, unsigned int idx, size_t n) {
			memset(chunk->buff + idx, 0, n);
		}

		template <typename T>
		static inline void Zero(Chunk* chunk, unsigned int idx,T* t) {
			Zero(chunk, idx, sizeof(T));
		}
	};
}