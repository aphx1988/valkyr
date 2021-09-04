#pragma once
#include "../Renderer.h"
#include "ResBuilder.h"
#include <vulkan/vulkan.h>
#include <string>

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::string error_str = "Detected Vulkan error: ";        \
			error_str.append(std::to_string(err));                                    \
			throw std::runtime_error(error_str);                    \
			abort();                                                \
		}                                                           \
	} while (0)

namespace valkyr {
	namespace render {
		class vkRenderer :public Renderer
		{
		public:
			vkRenderer():width(0),height(0){}
			~vkRenderer(){}

			void init(RenderSetting setting);
			void perFrame();
			void destroy();

		private:
			bool validateExts(const Vec<const char*>& required, const Vec<VkExtensionProperties>& available) {
				for (auto extension : required)
				{
					bool found = false;
					for (auto& available_extension : available)
					{
						if (strcmp(available_extension.extensionName, extension) == 0)
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						return false;
					}
				}
				return true;
			}

			VkInstance instance;

			unsigned width, height;

		};

	}
}