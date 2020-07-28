#pragma once
#include "../vcontainer/MultipleTypeSpan.h"

namespace valkyr {
	struct Entity {
		unsigned int id;
		unsigned int generation;
	};

	//can use its info for getting type
	struct CptChunkNode {
		unsigned int firstHead;
		size_t size;
		CptChunkNode* next;
		CptChunkNode* prev;
	};

	

	//arch's proxy in each chunk
	//multiple ACN can be in one chunk
	struct ArchChunkNode {
		Chunk* chunk;
		ChunkInfo* chunkInfo;
		size_t size;
		ArchChunkNode* next;
		ArchChunkNode* prev;
		//prototype cpts
		//Span<CptChunkNode>* firstCptSpan;
		int pad[2];
	};


	//template<typename ...Ts>
	//struct Arch;
	//
	//template <>
	//struct Arch<>{};
	//
	//template <typename Cpt,typename Cpts>
	//struct Arch<Cpt,Cpts>:Arch<Cpts> {
	//	ArchCptNode<Cpt> cpt;
	//
	//	Arch(Cpt cpt,Cpts... rest):Arch<Cpts...>(std::forward<Cpts>(rest)...),cpt(cpt){}
	//};

	struct Arch {
		ArchChunkNode* firstCpt;  //after init, create these cpts in chunk as template or ref
		unsigned int cptNum;
		Chunk* firstChunk;
		Chunk* lastChunkCanUse;  //unused size >= sizeof...(cpts)
	};

	class ArchUtil {

	};
}