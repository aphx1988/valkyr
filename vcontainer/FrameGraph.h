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
		FrameGraph():thread_count(0){}
		~FrameGraph(){}

		std::function<void()> fn_build;
		std::function<void()> fn_exec;
		Vec<Pass<P...>> pass_list;
		Vec<Res<R>> res_list;
		size_t thread_count;
		Map<unsigned, Vec<Pass<P...>>> thread_pass_map;
	};
	
}
