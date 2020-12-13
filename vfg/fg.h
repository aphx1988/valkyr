#pragma once
#include "../vcontainer/graph.h"
#include "../vcontainer/vec.h"
#include "../vcontainer/map.h"
#include "../vtask/Scheduler.h"
#include "cmd.h"

namespace valkyr {
	enum ResState {
		Created = 0 ,
		NeverCreated,
		Present, RenderTarget,SRV,UAV,CBV
	};

	enum class ResHeapType {
		RTV,DSV,SRV_UAV_CBV,Sampler
	};

	struct Res {
		unsigned id;
		unsigned format;
		unsigned type;
		unsigned typeListIdx;
		unsigned width;
		unsigned height;
		ResState state;
		unsigned userTag;
	};

	struct Pass {
		Vec<unsigned> inputs;
		Vec<unsigned> outputs;
		std::function<void(Vec<unsigned>, Vec<unsigned>)> setupFunc;
		std::function<void(FgBuilder builder,Vec<unsigned>,Vec<unsigned>)> renderFunc;
	};

	struct Fg {
	public:
		Vec<Res> resList;
		Vec<Pass> passList;
		Map<std::string_view, unsigned> passMap;

		Fg() {}
	};

	class FgBuilder {
	public:
		virtual void CreateRT(std::string_view name,unsigned format,unsigned downSampleRatio)=0;
		virtual void UseRT(std::string_view name)=0;
		//virtual void CreateUAV() = 0;
	};
}
