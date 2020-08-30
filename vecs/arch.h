#pragma once
#include "../vcontainer/pool.h"
#include "../vcontainer/tuple.h"

namespace valkyr {
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

	template <typename ...T>
	using Arch = Pool<Tuple<T...>>;

	template <typename ...T>
	class ArchMgr
	{
	public:
		void Create(size_t num) {

		}

		Arch<T...>* m_root;
	};

	
}