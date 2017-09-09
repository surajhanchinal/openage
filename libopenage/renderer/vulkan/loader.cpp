#include "loader.h"


namespace openage {
namespace renderer {
namespace vulkan {

VlkLoader::VlkLoader(VkInstance instance) {
	this->CreateDebugReportCallbackEXT = PFN_vkCreateDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
	this->DestroyDebugReportCallbackEXT = PFN_vkDestroyDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
}

VkResult VlkLoader::vkCreateDebugReportCallbackEXT(
	VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugReportCallbackEXT* pCallback) {
	if (this->CreateDebugReportCallbackEXT != nullptr) {
		return this->CreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VlkLoader::vkDestroyDebugReportCallbackEXT(
	VkInstance instance,
	VkDebugReportCallbackEXT callback,
	const VkAllocationCallbacks* pAllocator
) {
	if (this->DestroyDebugReportCallbackEXT != nullptr) {
		this->DestroyDebugReportCallbackEXT(instance, callback, pAllocator);
	}
}

}}} // openage::renderer::vulkan
