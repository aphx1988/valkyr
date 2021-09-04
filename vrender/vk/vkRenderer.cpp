#include "vkRenderer.h"
#include <stdexcept>

void valkyr::render::vkRenderer::init(RenderSetting setting)
{
	uint32_t extCount = 0;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr));
	Vec<VkExtensionProperties> extProps;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extProps.data()));
	Vec<const char*> activeExts = {};
#if defined _DEBUG || defined DEBUG
	activeExts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#ifdef VK_USE_PLATFORM_WIN32_KHR
	activeExts.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif  VK_USE_PLATFORM_ANDROID_KHR
	activeExts.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif  VK_USE_PLATFORM_XCB_KHR
	activeExts.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif  VK_USE_PLATFORM_DISPLAY_KHR
	activeExts.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#else
#pragma error "Platform not supported"
#endif
#endif
	if (extProps.size() == 0) {
		//activeExts.clear();
	}
	else if (!validateExts(activeExts, extProps))
	{
		throw std::runtime_error("Required instance extensions are missing.");
	}
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
	appInfo.pApplicationName = setting.title.data();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "valkyr";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;
	appInfo.pNext = nullptr;
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = activeExts.size();
	createInfo.ppEnabledExtensionNames = activeExts.data();
	createInfo.enabledLayerCount = 0;
	VK_CHECK(vkCreateInstance(&createInfo,nullptr, &m_instance));
	uint32_t deviceCount=0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("no vulkan physical device");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
	VkPhysicalDeviceProperties prop;
	VkPhysicalDeviceFeatures feature;
	for (const auto& dev : devices) {
		VkPhysicalDeviceProperties deviceProps;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(dev, &deviceProps);
		vkGetPhysicalDeviceFeatures(dev, &deviceFeatures);
		if (checkDevice(dev,deviceProps, deviceFeatures)) {
			m_phy_device = dev;
			prop = deviceProps;
			break;
		}
	}
	if (m_phy_device == VK_NULL_HANDLE) {
		throw std::runtime_error("no suitable vulkan physical device");
	}
	VkDeviceQueueCreateInfo queue_info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queue_info.queueFamilyIndex = m_g_queue_family_index;
	queue_info.queueCount = 1;
	float queue_priority = 1.0f;
	queue_info.pQueuePriorities = &queue_priority;

	VkDeviceCreateInfo device_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledExtensionCount = activeExts.size();
	device_info.ppEnabledExtensionNames = activeExts.data();
	vkCreateDevice(m_phy_device, &device_info, nullptr, &m_device);
	vkGetDeviceQueue(m_device, m_g_queue_family_index, 0, &m_g_queue);
	int x = 0;
}

void valkyr::render::vkRenderer::perFrame()
{
}

void valkyr::render::vkRenderer::destroy()
{
	vkDestroyInstance(m_instance, nullptr);
}