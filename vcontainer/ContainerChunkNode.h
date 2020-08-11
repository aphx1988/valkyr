#pragma once

//#include "span.h"
#include "algorithm.h"

namespace valkyr {
	const int NUM_PRECREATED = 8;
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
		Span<T>* lastZeroSpan;
		unsigned int spanCount;
		T* lastZero;

		ContainerChunkNode():prev(nullptr),next(nullptr),chunkInfo(nullptr),
			chunk(nullptr),spanCount(0),firstSpan(nullptr),lastZeroSpan(nullptr){}
	};

	class ContainerChunkNodeUtil {
	public:
		template <typename T>
		static inline Span<T>* NewSpan(ContainerChunkNode<T>* node,int baseId) {
			Span<T>* span = SpanUtil::Create<T>(node->chunk, NUM_PRECREATED,baseId);
			return span;
		}

		//check if node's chunk has zero T,or create one with a new span
		/*template <typename T>
		static T* PickZero(ContainerChunkNode<T>* node,int baseId,std::function<void()> onNewSpan) {
			Span<T>* span = node->firstSpan;
			T* azero = nullptr;
			while (span != nullptr && azero == nullptr) {
				azero = vpick<Span<T>, T>(span,
					[&](auto* curr) { return SpanUtil::PickZero(curr); });
				span = span->next;
			}
			if(azero!=nullptr)
				return azero;
			span = NewSpan(node,baseId,onNewSpan);
			if (span != nullptr) {
				azero =  SpanUtil::Get(0, span);
			}
			return azero;
		}*/

		template <typename T>
		static std::pair<T*,SpanEntity*> PickZeroWithEntity(ContainerChunkNode<T>* node, int baseId, std::function<void(Span<T>*)> onNewSpan) {
			Span<T>* span = node->lastZeroSpan?node->lastZeroSpan:node->firstSpan;
			Span<T>* lastUsedSpan = nullptr;
			std::pair<T*, SpanEntity*> azero;
			while (span != nullptr && azero.first == nullptr) {
				azero = vpick_with_entity<Span<T>, T>(span,
					[&](Span<T>* curr) { return SpanUtil::PickZeroWithEntity(curr); });
				lastUsedSpan = span;
				span = span->next;
			}
			if (azero.first != nullptr) {
				node->lastZeroSpan = lastUsedSpan;
				return azero;
			}
			span = NewSpan<T>(node, baseId);
			if (span != nullptr) {
				node->spanCount++;
				node->lastZeroSpan = span;
				onNewSpan(span);
				azero = SpanUtil::GetWithEntity<T>(0, span);
				if(lastUsedSpan) lastUsedSpan->next = span;
				azero.second->isZero = false;
			}
			return azero;
		}
	};
}
