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

	struct RenderSetting {
		unsigned width;
		unsigned height;
		unsigned num_fb;
		bool fullscreen;
		bool software;
	};

	template<class R,class ...P>
	class FrameGraph
	{
	public:
		FrameGraph(){}
		~FrameGraph(){}

		virtual void init(RenderSetting setting) = 0;
		virtual void build()=0;
		virtual void exec()=0;

	private:
		Vec<Pass<P...>> pass_list;
		Vec<Res<R>> res_list;
		size_t thread_count;
		Map<unsigned, Vec<Pass<P...>>> thread_pass_map;
	};
	
}
