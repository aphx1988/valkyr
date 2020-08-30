#pragma once

#include <functional>
#include "tuple.h"

namespace valkyr{
	//a continuous or linear memory store a type in a chunk
	//can be multiple in a single chunk, so has prev and next for connection
	//all elements are fixed, can not add more or remove

	template <typename T>
	struct Span {
		size_t firstHead;
		size_t length;
 		Chunk* chunk;
		Span<T>* next;
		Span<T>* prev;
		size_t pad[3] = { 0, 0, 0 };

		Span() : firstHead(0), length(0),chunk(nullptr), 
			next(nullptr), prev(nullptr)
		{
		}

		Span(Chunk* chunk) :
			length(0), chunk(chunk), next(nullptr), prev(nullptr) {
			firstHead = ChunkUtil::GetInfo(chunk)->head;
		}
	};

	class SpanUtil {
	public:

		template <typename T, typename ...Args>
		static Span<T>* Create(Chunk* chunk, int num, Args... args) {
			if (!CanCreate<T>(chunk,num)) return nullptr;
			Span<T>* span = ChunkUtil::NewObjFrom<Span<T>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			for (int i = 0; i < num; i++) {
				ChunkUtil::NewObjFrom<T>(chunk,args...);
			}
			span->length = num;
			return span;
		}

		template <typename T>
		static Span<T>* Create(Chunk* chunk, int num) {
			if (!CanCreate<T>(chunk, num)) return nullptr;
			Span<T>* span = ChunkUtil::NewObjFrom<Span<T>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			for (int i = 0; i < num; i++) {
				ChunkUtil::NewObjFrom<T>(chunk);
			}
			span->length = num;
			return span;
		}

		template <typename ...T>
		static inline bool CanCreate(Chunk* chunk, int num) {
			size_t elementSize = sizeof(Tuple<T...>);
			size_t spanSize = sizeof(Span<Tuple<T...>>);
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= elementSize * num + spanSize;
		}


		template <typename T>
		static inline T* Get(unsigned int idx, Span<T>* span) {
			if (!span) return nullptr;
			size_t tsize = sizeof(T);
			return ChunkUtil::Get<T>(span->firstHead + idx * tsize, span->chunk);
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
			for (int i = 0; i < span->length; ++i) {
				func(Get(i,span),i);
			}
		}

		template <typename T>
		static void Zero(Span<T>* span, int idx) {
			size_t tsize = sizeof(T);
			ChunkUtil::Zero(span->chunk, span->firstHead + idx * tsize,sizeof(T));
		}

		template <typename T>
		static void ZeroRange(Span<T>* span, int startIdx, size_t n) {
			//ChunkUtil::Zero(span->chunk, span->firstHead + startIdx * sizeof(T), (n > span->count ? span->count : n) * sizeof(T));
			//zero data but not span entity
			for (int i = 0; i < n; i++) {
				SpanUtil::Zero(span, startIdx+i);
			}
		}

		template <typename T>
		static void ZeroAll(Span<T>* span) {
			SpanUtil::ZeroRange(span, 0, span->length);
		}
	};
}
