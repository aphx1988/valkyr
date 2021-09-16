#include "vkRenderer.h"
#include <stdexcept>

void valkyr::render::vkRenderer::init(RenderSetting setting)
{
	uint32_t extCount = 0;
	const char** activeExts;
	activeExts = glfwGetRequiredInstanceExtensions(&extCount);
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
	createInfo.enabledExtensionCount = extCount;
	createInfo.ppEnabledExtensionNames = activeExts;
	createInfo.enabledLayerCount = 0;
#ifdef _DEBUG
	VkDebugReportCallbackCreateInfoEXT debug_report_create_info = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
	debug_report_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debug_report_create_info.pfnCallback = debug_callback;
	createInfo.pNext = &debug_report_create_info;
	
#endif
	VK_CHECK(vkCreateInstance(&createInfo,nullptr, &m_instance));
	m_window = (GLFWwindow*)setting.window;
	VkWin32SurfaceCreateInfoKHR sufCreateInfo{};
	sufCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	sufCreateInfo.hwnd = glfwGetWin32Window(m_window);
	sufCreateInfo.hinstance = GetModuleHandle(nullptr);
	VK_CHECK(vkCreateWin32SurfaceKHR(m_instance, &sufCreateInfo, nullptr, &m_surface));
	initDevice(activeExts,extCount);
	//initSwapchain();
	int x = 0;
}

void valkyr::render::vkRenderer::perFrame()
{
}

void valkyr::render::vkRenderer::destroy()
{
	vkDestroyInstance(m_instance, nullptr);
}

void valkyr::render::vkRenderer::initDevice(const char** activeExts,uint32_t extCounts)
{
	uint32_t deviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr));
	if (deviceCount == 0) {
		throw std::runtime_error("no vulkan physical device");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()));
	m_phy_device = findPhyDevice(devices);
	uint32_t device_extension_count = 0;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(m_phy_device, nullptr, &device_extension_count, nullptr));
	std::vector<VkExtensionProperties> device_extensions(device_extension_count);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(m_phy_device, nullptr, &device_extension_count, device_extensions.data()));
	/*if (!validateExts(activeExts, device_extensions))
	{
		throw std::runtime_error("required extensions not support");
	}*/
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_phy_device, &queue_family_count, nullptr);
	if (queue_family_count == 0) {
		throw std::runtime_error("no queue family");
	}
	std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_phy_device, &queue_family_count, queue_family_properties.data());
	for (uint32_t i = 0; i < queue_family_count; i++)
	{
		VkBool32 supports_present= VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_phy_device, i, m_surface, &supports_present);

		// Find a queue family which supports graphics and presentation.
		if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supports_present)
		{
			m_g_queue_family_index = i;
		}
		if (queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			m_c_queue_family_index = i;
			break;
		}
	}
	VkDeviceQueueCreateInfo g_queue_info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	g_queue_info.queueFamilyIndex = m_g_queue_family_index;
	g_queue_info.queueCount = 1;
	float g_queue_priority = 1.0f;
	g_queue_info.pQueuePriorities = &g_queue_priority;
	VkDeviceQueueCreateInfo c_queue_info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	c_queue_info.queueFamilyIndex = m_c_queue_family_index;
	c_queue_info.queueCount = 1;
	float c_queue_priority = 1.0f;
	c_queue_info.pQueuePriorities = &c_queue_priority;
	Vec<VkDeviceQueueCreateInfo> queueCreateInfo;
	queueCreateInfo.emplace_back(g_queue_info);
	queueCreateInfo.emplace_back(c_queue_info);
	VkDeviceCreateInfo device_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	device_info.queueCreateInfoCount = 2;
	device_info.pQueueCreateInfos = queueCreateInfo.data();
	device_info.enabledExtensionCount = extCounts;
	device_info.ppEnabledExtensionNames = activeExts;
	vkCreateDevice(m_phy_device, &device_info, nullptr, &m_device);
	vkGetDeviceQueue(m_device, m_g_queue_family_index, 0, &m_g_queue);
}

void valkyr::render::vkRenderer::initSwapchain()
{
	VkSurfaceCapabilitiesKHR surface_properties;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_phy_device, m_surface, &surface_properties));

	uint32_t format_count=3;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_phy_device, m_surface, &format_count, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_phy_device, m_surface, &format_count, formats.data());

	VkSurfaceFormatKHR format;
	if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		// There is no preferred format, so pick a default one
		format = formats[0];
		format.format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		if (format_count == 0)
		{
			throw std::runtime_error("Surface has no formats.");
		}

		format.format = VK_FORMAT_UNDEFINED;
		for (auto& candidate : formats)
		{
			switch (candidate.format)
			{
			case VK_FORMAT_R8G8B8A8_UNORM:
			case VK_FORMAT_B8G8R8A8_UNORM:
			case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
				format = candidate;
				break;

			default:
				break;
			}

			if (format.format != VK_FORMAT_UNDEFINED)
			{
				break;
			}
		}

		if (format.format == VK_FORMAT_UNDEFINED)
		{
			format = formats[0];
		}
	}

	VkExtent2D swapchain_size = surface_properties.currentExtent;

	// FIFO must be supported by all implementations.
	VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;

	// Determine the number of VkImage's to use in the swapchain.
	// Ideally, we desire to own 1 image at a time, the rest of the images can
	// either be rendered to and/or being queued up for display.
	uint32_t desired_swapchain_images = surface_properties.minImageCount + 1;
	if ((surface_properties.maxImageCount > 0) && (desired_swapchain_images > surface_properties.maxImageCount))
	{
		// Application must settle for fewer images than desired.
		desired_swapchain_images = surface_properties.maxImageCount;
	}

	// Figure out a suitable surface transform.
	VkSurfaceTransformFlagBitsKHR pre_transform;
	if (surface_properties.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		pre_transform = surface_properties.currentTransform;
	}

	VkSwapchainKHR old_swapchain = m_swapchain;

	// Find a supported composite type.
	VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}
	else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	}
	else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
	}
	else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
	{
		composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
	}

	VkSwapchainCreateInfoKHR info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	info.surface = m_surface;
	info.minImageCount = desired_swapchain_images;
	info.imageFormat = format.format;
	info.imageColorSpace = format.colorSpace;
	info.imageExtent.width = swapchain_size.width;
	info.imageExtent.height = swapchain_size.height;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.preTransform = pre_transform;
	info.compositeAlpha = composite;
	info.presentMode = swapchain_present_mode;
	info.clipped = true;
	info.oldSwapchain = old_swapchain;

	VK_CHECK(vkCreateSwapchainKHR(m_device, &info, nullptr, &m_swapchain));

	if (old_swapchain != VK_NULL_HANDLE)
	{
		for (VkImageView image_view : m_swapchain_imageviews)
		{
			vkDestroyImageView(m_device, image_view, nullptr);
		}

		uint32_t image_count;
		VK_CHECK(vkGetSwapchainImagesKHR(m_device, old_swapchain, &image_count, nullptr));

		for (size_t i = 0; i < image_count; i++)
		{
			teardownFrame(m_frame_info_list[i]);
		}

		m_swapchain_imageviews.clear();

		vkDestroySwapchainKHR(m_device, old_swapchain, nullptr);
	}

	m_width = swapchain_size.width;
	m_height = swapchain_size.height; 
	m_swapchain_format = format.format ;

	uint32_t image_count;
	VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr));

	/// The swapchain images.
	std::vector<VkImage> swapchain_images(image_count);
	VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, swapchain_images.data()));

	// Initialize per-frame resources.
	// Every swapchain image has its own command pool and fence manager.
	// This makes it very easy to keep track of when we can reset command buffers and such.
	m_frame_info_list.clear();
	m_frame_info_list.resize(image_count);

	for (size_t i = 0; i < image_count; i++)
	{
		initFrame(m_frame_info_list[i]);
	}

	for (size_t i = 0; i < image_count; i++)
	{
		// Create an image view which we can render into.
		VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format =m_swapchain_format;
		view_info.image = swapchain_images[i];
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.layerCount = 1;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.components.r = VK_COMPONENT_SWIZZLE_R;
		view_info.components.g = VK_COMPONENT_SWIZZLE_G;
		view_info.components.b = VK_COMPONENT_SWIZZLE_B;
		view_info.components.a = VK_COMPONENT_SWIZZLE_A;

		VkImageView image_view;
		VK_CHECK(vkCreateImageView(m_device, &view_info, nullptr, &image_view));

		m_swapchain_imageviews.push_back(image_view);
	}
}

void valkyr::render::vkRenderer::initFrame(vkFrameInfo& frameInfo)
{
	VkFenceCreateInfo info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	VK_CHECK(vkCreateFence(m_device, &info, nullptr, &frameInfo.fence));

	VkCommandPoolCreateInfo cmd_pool_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	cmd_pool_info.queueFamilyIndex = m_g_queue_family_index;
	VK_CHECK(vkCreateCommandPool(m_device, &cmd_pool_info, nullptr, &frameInfo.cmd_pool));

	VkCommandBufferAllocateInfo cmd_buf_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	cmd_buf_info.commandPool = frameInfo.cmd_pool;
	cmd_buf_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd_buf_info.commandBufferCount = 1;
	VK_CHECK(vkAllocateCommandBuffers(m_device, &cmd_buf_info, &frameInfo.cmd_buff));
}

void valkyr::render::vkRenderer::teardownFrame(vkFrameInfo& frameInfo)
{
	if (frameInfo.fence != VK_NULL_HANDLE) {
		vkDestroyFence(m_device, frameInfo.fence, nullptr);
		frameInfo.fence = VK_NULL_HANDLE;
	}
	if (frameInfo.cmd_buff != VK_NULL_HANDLE) {
		vkFreeCommandBuffers(m_device, frameInfo.cmd_pool,1,&frameInfo.cmd_buff);
		frameInfo.cmd_buff = VK_NULL_HANDLE;
	}
	if (frameInfo.cmd_pool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(m_device, frameInfo.cmd_pool, nullptr);
		frameInfo.cmd_pool = VK_NULL_HANDLE;
	}
	if (frameInfo.sp_acquire != VK_NULL_HANDLE) {
		vkDestroySemaphore(m_device, frameInfo.sp_acquire, nullptr);
		frameInfo.sp_acquire = VK_NULL_HANDLE;
	}
	if (frameInfo.sp_release != VK_NULL_HANDLE) {
		vkDestroySemaphore(m_device, frameInfo.sp_release, nullptr);
		frameInfo.sp_release = VK_NULL_HANDLE;
	}
}
