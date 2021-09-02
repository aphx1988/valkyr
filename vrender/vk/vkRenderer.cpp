#include "vkRenderer.h"

void valkyr::render::vkRenderer::init(RenderSetting setting)
{
	uint32_t extCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
	Vec<VkExtensionProperties> extProps;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extProps.data);
}

void valkyr::render::vkRenderer::build()
{
}

void valkyr::render::vkRenderer::exec()
{
}