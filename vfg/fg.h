#pragma once
//#include "../vcontainer/graph.h"
#include "../vcontainer/vec.h"
#include "../vcontainer/map.h"
#include "../vtask/Scheduler.h"
#include "cmd.h"
#include <string_view>

namespace valkyr {
	enum ResState {
		Created = 0 ,
		NeverCreated,
		Present, RenderTarget,SRV,UAV,CBV
	};

	enum class ResHeapType {
		RTV,DSV,SRV_UAV_CBV,Sampler
	};

  enum class ResActionType{
    Create,Destroy,Read,Write
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
  
  struct ResNode {
    unsigned resId;
    unsigned action;
  };

	class FgBuilder {
	public:
		virtual void CreateRT(std::string_view name, unsigned format, unsigned downSampleRatio) = 0;
		virtual void Read(std::string_view name) = 0;
    virtual void Read(unsigned id) = 0;
    virtual void Write(std::string_view name) = 0; 
    virtual void Write(unsigned id) = 0;
    virtual void ReadTemp(std::string_view name) = 0;
    virtual void ReadTemp(unsigned id) = 0;
    virtual void WriteTemp(std::string_view name) = 0; 
    virtual void WriteTemp(unsigned id) = 0;
		//virtual void CreateUAV() = 0;
	};

	struct Pass {
    unsigned id;
		std::string_view name;
		/*Vec<unsigned> inputs;
		Vec<unsigned> outputs;*/
		std::function<void(FgBuilder& builder)> setupFunc;
		std::function<void(FgBuilder& builder, CmdList& cmd,Vec<unsigned>,Vec<unsigned>)> renderFunc;

		//builder调用pass的settup后生成pass，并在fg中更新依赖，先生成的pass在fg中先出现
		Pass(std::string_view tagName, std::function<void(FgBuilder& builder)> setupFun, std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>, Vec<unsigned>)> renderFun) {
			setupFunc = std::move(setupFun);
			renderFunc = std::move(renderFun);
			name = tagName;
		}
	};

 // struct PassNode {
 //   unsigned passId;
 //   Vec<unsigned> readTempList;
 //   Vec<unsigned> readList;
 //   Vec<unsigned> writeTempList;
 //   Vec<unsigned> writeList;
 // };

	struct Fg {
	public:
		Vec<Res> tempResList;
		Vec<Res> importedResList;
		Vec<Pass> passList;
		Map<std::string_view,Pass> passMap;
    Map<std::string_view,Res> resMap;

    Map<unsigned,ResNode> passResMap;

		Fg() {}
	};

	
}
