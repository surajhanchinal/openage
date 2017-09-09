#pragma once

#include <vulkan/vulkan.hpp>


namespace openage {
namespace renderer {
namespace vulkan {

/// A class for dynamically loading Vulkan extension functions.
class VlkLoader {
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallbackEXT;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallbackEXT;

public:
	/// Initialize this loader for the given Vulkan instance.
	VlkLoader(VkInstance instance);

	/// Part of VK_EXT_debug_report, allows setting a callback for debug events.
	vk::Result createDebugReportCallbackEXTUnique(
		vk::Instance instance,
		const vk::DebugReportCallbackCreateInfoEXT pCreateInfo,
		const vk::AllocationCallbacks* pAllocator,
		vk::DebugReportCallbackEXT* pCallback
	);


	/// Part of VK_EXT_debug_report, destroys the debug callback object.
	void vkDestroyDebugReportCallbackEXT(
		VkInstance instance,
		VkDebugReportCallbackEXT callback,
		const VkAllocationCallbacks* pAllocator
	);
};

}}} // openage::renderer::vulkan
