#pragma once
#include "vec.h"
#include "map.h"
#include "../vcore/vptr.h"
#include <functional>

using std::function;

namespace valkyr {
	template<class ...T>
	struct Pass {
		unsigned id;
		unsigned next;
		function<void(T...)> setup;
		function<void(T...)> exec;
	};

	template<class T>
	struct Res
	{
		unsigned id;
		unsigned pass_id;
		unsigned type_id;
		T data;
	};

	template<class R,class ...P>
	struct FrameGraph
	{
	public:
		FrameGraph():m_thread_count(0){}
		~FrameGraph(){}

		std::function<void()> m_fn_build;
		std::function<void()> m_fn_exec;
		Vec<Pass<P...>> m_pass_list;
		Vec<Res<R>> m_res_list;
		size_t m_thread_count;
		Map<unsigned, Vec<Pass<P...>>> m_thread_pass_map;
	};
	
}
