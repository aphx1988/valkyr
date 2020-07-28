#pragma once

#include <functional>
#include "tuple.h"

namespace valkyr{
	//a continuous or linear memory store a type in a chunk
	//can be multiple in a single chunk, so has prev and next for connection
	//all elements are fixed, can not add more or remove

	template <typename T>
	struct Span {
		unsigned int firstHead;
		unsigned int count;
		Chunk* chunk;
		Span<T>* next;
		Span<T>* prev;

		Span() : firstHead(0), count(0), chunk(nullptr), next(nullptr), prev(nullptr)
		{
		}

		Span(Chunk* chunk) :
			count(0), chunk(chunk), next(nullptr), prev(nullptr) {
			firstHead = ChunkUtil::GetInfo(chunk)->head;
		}
	};

	class SpanUtil {
	public:

		template <typename ...T>
		static inline Span<vtuple<T...>>* Create(Chunk* chunk,int num) {
			if (!CanCreate<T...>(chunk,num)) return nullptr;
			Span<vtuple<T...>>* span = ChunkUtil::NewObjFrom<Span<vtuple<T...>>>(chunk);
			span->chunk = chunk;
			//then all types
			//in memory:span-types...-types...
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			for (int i = 0; i < num; i++) {
				vmake_tuple<T...>(chunk);
			}
			span->count = num;
			return span;
		}

		template <typename ...T>
		static inline Span<vtuple<T...>>* Create(Chunk* chunk, int num, T... prototype) {
			if (!CanCreate<T...>(chunk, num)) return nullptr;
			Span<vtuple<T...>>* span = ChunkUtil::NewObjFrom<Span<vtuple<T...>>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			//then all types
			//in memory:span-types...-types...
			for (int i = 0; i < num; i++) {
				vmake_tuple<T...>(chunk,prototype...);
			}
			span->count = num;
			return span;
		}

		template <typename T, typename ...Args>
		static inline Span<T>* Create(Chunk* chunk, int num, Args... args) {
			if (!CanCreate<T>(chunk,num)) return nullptr;
			Span<T>* span = ChunkUtil::NewObjFrom<Span<T>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			//then all types
			//in memory:span-types...-types...
			for (int i = 0; i < num; i++) {
				ChunkUtil::NewObjFrom<T>(chunk,args...);
			}
			span->count = num;
			return span;
		}

		template <typename T>
		static inline Span<T>* Create(Chunk* chunk, int num) {
			if (!CanCreate<T>(chunk, num)) return nullptr;
			Span<T>* span = ChunkUtil::NewObjFrom<Span<T>>(chunk);
			span->chunk = chunk;
			span->firstHead = ChunkUtil::GetInfo(chunk)->head;
			//then all types
			//in memory:span-types...-types...
			for (int i = 0; i < num; i++) {
				ChunkUtil::NewObjFrom<T>(chunk);
			}
			span->count = num;
			return span;
		}

		template <typename ...T>
		static inline bool CanCreate(Chunk* chunk, int num) {
			size_t elementSize = sizeof(vtuple<T...>);
			size_t spanSize = sizeof(Span<vtuple<T...>>);
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= elementSize * num + spanSize;
		}

		template <typename T>
		static inline bool CanCreate(Chunk* chunk,int num) {
			size_t elementSize = sizeof(T);
			size_t spanSize = sizeof(Span<vtuple<T...>>);
			ChunkInfo* info = ChunkUtil::GetInfo(chunk);
			return CHUNK_SIZE - info->usedSize >= elementSize*num + spanSize;
		}

		template <typename T>
		static inline T* Get(unsigned int idx, Span<T>* span) {
			return ChunkUtil::Get<T>(span->firstHead + idx * sizeof(T), span->chunk);
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
		}
	};
}
