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
			vkRenderer():m_width(0),m_height(0){}
			~vkRenderer(){}

			void init(RenderSetting setting);
			void perFrame();
			void destroy();

		private:
			bool validateExts(const Vec<const char*>& required, const Vec<VkExtensionProperties>& available) {
				for (auto extension : required)
				{
					bool found = false;
					for (const auto& available_extension : available)
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

			bool checkDevice(const VkPhysicalDevice& device,const VkPhysicalDeviceProperties& deviceProps,const VkPhysicalDeviceFeatures& deviceFeatures) {
				uint32_t queueFamilyCount = 0;
				Vec<VkQueueFamilyProperties> queueFamilyProps;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProps.data());
				if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM || deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER) {
					return false;
				}
				for (int i = 0; i < queueFamilyProps.size();i++) {
					const auto& prop = queueFamilyProps[i];
					if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						m_g_queue_family_index = i;
						if (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) {
							m_g_queue_family_index = i;
							break;
						}
						else {
							return false;
						}
					}
				}
				return true;
			}

			VkInstance m_instance = VK_NULL_HANDLE;
			VkPhysicalDevice m_phy_device = VK_NULL_HANDLE;
			VkDevice m_device = VK_NULL_HANDLE;
			VkQueue m_g_queue = VK_NULL_HANDLE;
			VkQueue m_c_queue = VK_NULL_HANDLE;
			uint32_t m_g_queue_family_index = 0;
			uint32_t m_c_queue_family_index = 0;

			unsigned m_width, m_height;

		};

	}
}