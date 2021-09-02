#pragma once
#include "../RenderGraph.h"
#include "ResBuilder.h"
#include <vulkan/vulkan.h>

namespace valkyr {
	namespace render {
		class vkRenderer :RenderGraph
		{
		public:
			vkRenderer(){}
			~vkRenderer(){}

			void init(RenderSetting setting);
			void build();
			void exec();

		private:
			
		};

	}
}