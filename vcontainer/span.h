#pragma once

#include <functional>
#include "./tuple.h"

namespace valkyr{
	//a continuous or linear memory store a type in a chunk
	//can be multiple in a single chunk, so has prev and next for connection
	//all elements are fixed, can not add more or remove

	template <typename T>
	struct Span {
		unsigned int startIdx;
		unsigned int count;
		Chunk* chunk;
		Span<T>* next;
		Span<T>* prev;

		Span() : startIdx(0), count(0), chunk(nullptr), next(nullptr), prev(nullptr)
		{
		}

		Span(Chunk* chunk) :
			count(0), chunk(chunk), next(nullptr), prev(nullptr) {
			startIdx = ChunkUtil::GetInfo(chunk)->head;
		}
	};

	class SpanUtil {
	public:
		template <typename ...T>
		static inline Span<T...>* Create(Chunk* chunk, T&&... ptototypes,int num) {
			if (!CanCreate<T...>(chunk)) return;
			Span<T...>* span = ChunkUtil::NewObjFrom<Span<T...>>(chunk);
			span->chunk = chunk;
			//then all types
			//in memory:span-types...-types...

			return span;
		}

		template <typename ...T>
		static inline bool CanCreate(Chunk* chunk) {
			size_t elementSize = Span<T...>::ElementSize;
			size_t spanSize = sizeof(Span<T...>);
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= elementSize + spanSize;
		}


		/*template <typename T>
		static inline T* Get(unsigned int idx, Span<T>* span) {
			return ChunkUtil::GetFrom<T>(span->startIdx + idx * sizeof(T), span->chunk);
		}

		template <typename T>
		static inline void Connect(Span<T>* span, Span<T>* next) {
			if (span->chunk == next->chunk) {
				span->next = next;
				next->prev = span;
			}
		}

		template <typename T>
		static inline void ForEach(Span<T>* span, std::function<void(T*,int)> func) {
			for (int i = 0; i < span->count; ++i) {
				func(Get(i,span),i);
			}
		}*/
	};
}
