#pragma once
#include "../vmalloc/chunk.h"

namespace valkyr {
	struct Entity {
		unsigned int id;
		unsigned int generation;
	};

	//template<typename T>
	struct ArchCptNode {
		//T* cpt;
		void* cpt;
		ArchCptNode* next;
		ArchCptNode* prev;
		int pad;
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
		ArchCptNode* firstCpt;  //after init, create these cpts in chunk as template or ref
		unsigned int cptNum;
		Chunk* firstChunk;
		Chunk* lastChunkCanUse;  //unused size >= sizeof...(cpts)
	};
}