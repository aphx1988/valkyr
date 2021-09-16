#pragma once
#include "../Renderer.h"
#include "ResBuilder.h"
#include <vulkan/vulkan.h>
#include <string>
#ifdef _WIN32
//#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_win32.h>
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


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

#if defined(_DEBUG)
/// @brief A debug callback called from Vulkan validation layers.
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
	uint64_t object, size_t location, int32_t message_code,
	const char* layer_prefix, const char* message, void* user_data)
{
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		//LOGE("Validation Layer: Error: {}: {}", layer_prefix, message);
	}
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		//LOGE("Validation Layer: Warning: {}: {}", layer_prefix, message);
	}
	else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		//LOGI("Validation Layer: Performance warning: {}: {}", layer_prefix, message);
	}
	else
	{
		//LOGI("Validation Layer: Information: {}: {}", layer_prefix, message);
	}
	return VK_FALSE;
}
#endif

namespace valkyr {
	namespace render {

		struct vkFrameInfo{
			VkFence fence;
			VkCommandPool cmd_pool;
			VkCommandBuffer cmd_buff;
			VkSemaphore sp_acquire;
			VkSemaphore sp_release;
		};

		class vkRenderer :public Renderer
		{
		public:
			vkRenderer():m_width(0),m_height(0){}
			~vkRenderer(){}

			void init(RenderSetting setting);
			void perFrame();
			void destroy();

		private:
			void initDevice(const char** activeExts, uint32_t extCounts);
			void initSwapchain();
			void initFrame(vkFrameInfo& frameInfo);
			void teardownFrame(vkFrameInfo& frameInfo);

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

			bool checkLayers() {
				const std::vector<const char*> validationLayers = {
					"VK_LAYER_KHRONOS_validation"
				};
				uint32_t layerCount;
				VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
				std::vector<VkLayerProperties> availableLayers(layerCount);
				VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

			}

			VkPhysicalDevice findPhyDevice(std::vector<VkPhysicalDevice>& devices) {
				return devices[0];
			}

			VkInstance m_instance = VK_NULL_HANDLE;
			VkPhysicalDevice m_phy_device = VK_NULL_HANDLE;
			VkDevice m_device = VK_NULL_HANDLE;
			VkSurfaceKHR m_surface = VK_NULL_HANDLE;
			VkQueue m_g_queue = VK_NULL_HANDLE;
			VkQueue m_c_queue = VK_NULL_HANDLE;
			uint32_t m_g_queue_family_index = 0;
			uint32_t m_c_queue_family_index = 0;
			VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
			Vec<VkImageView> m_swapchain_imageviews;
			Vec<VkFramebuffer> m_swapchain_framebuffers;
			unsigned m_width, m_height;
			VkFormat m_swapchain_format;
			Vec<vkFrameInfo> m_frame_info_list;
			int m_queue_index;
			GLFWwindow* m_window;
		};

	}
}