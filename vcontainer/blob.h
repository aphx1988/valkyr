#pragma once

#include "../vmalloc/chunk.h"

namespace valkyr {
	template <typename T>
	struct Part {
		T* data;
		Part* next;
		Chunk* chunk;
 		size_t size;
	};

	//blob∫Õ∆‰part∂¿’ºchunk
	template <typename T>
	struct Blob {
		Part<T>* firstPart;
		Part<T>* lastPart;
		Chunk* createdChunk;
		size_t numParts;
		size_t size;
		size_t pad[3] = { 0,0,0 };
	};

	class BlobUtil {
	public:
		template <typename T>
		static bool CanCreatePart(Chunk* chunk) {
			auto* info = ChunkUtil::GetInfo(chunk);
			bool r = CHUNK_SIZE - info->usedSize >= sizeof(Part<T>)+sizeof(T);
			return r;
		}

		template <typename T>
		static Blob<T>* Create(Chunk* firstChunk) {
			Blob<T>* blob = ChunkUtil::NewObjFrom<Blob<T>>(firstChunk);
			blob->firstPart = nullptr;
			blob->lastPart = nullptr;
			blob->size = 0;
			blob->numParts = 0;
			blob->createdChunk = firstChunk;
		}

		template <typename T>
		static Blob<T>* Create(Chunk* firstChunk,size_t size) {
			Blob<T>* blob = ChunkUtil::NewObjFrom<Blob<T>>(firstChunk);
			auto currSize = 0;
			auto numParts = 0;
			Chunk* currChunk = firstChunk;
			Part<T>* lastPart = nullptr;
			while (currSize < size) {
				/*if (!CanCreatePart<T>(currChunk)) {
					currChunk = ChunkAllocator::Malloc();
				}*/
				size_t head = ChunkUtil::GetInfo(currChunk)->head;
				Part<T>* part = ChunkUtil::NewObjFrom<Part<T>>(currChunk);
				T* data = ChunkUtil::NewObjFrom<T>(currChunk);
				part->data = data;
				part->chunk = currChunk;
				part->size = ChunkUtil::GetInfo(currChunk)->usedSize - head;
				lastPart->next = part;
				if (i == 0) blob->firstPart = part;
			}
			blob->size = size;
			blob->numParts = numParts;
			blob->createdChunk = firstChunk;
		}

		template <typename T>
		static Part<T>* NewPart(Blob<T>* blob) {
			Chunk* currChunk = blob->firstPart;
			/*if (!CanCreatePart<T>(currChunk)) {
				currChunk = ChunkAllocator::Malloc();
			}*/
			size_t head = ChunkUtil::GetInfo(currChunk)->head;
			Part<T>* part = ChunkUtil::NewObjFrom<Part<T>>(currChunk);
			T* data = ChunkUtil::NewObjFrom<T>(currChunk);
			part->data = data;
			part->chunk = currChunk;
			part->size = ChunkUtil::GetInfo(currChunk)->usedSize - head;
			blob->lastPart->next = part;
			blob->lastPart = part;
			if (blob->firstPart == nullptr) blob->firstPart = part;
			return part;
		}
	};
}
