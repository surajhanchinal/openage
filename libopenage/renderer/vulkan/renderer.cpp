#include "renderer.h"


#include "../../error/error.h"

#include "util.h"
#include "graphics_device.h"
#include "render_target.h"


namespace openage {
namespace renderer {
namespace vulkan {

void VlkRenderer::do_the_thing() {
	// Enumerate available physical devices
	auto devices = vk_do_ritual(vkEnumeratePhysicalDevices, this->instance);
	if (devices.size() == 0) {
		throw Error(MSG(err) << "No Vulkan devices available.");
	}

	std::vector<std::pair<VkPhysicalDevice, SurfaceSupportDetails>> support_per_dev;
	for (auto dev : devices) {
		auto support = VlkGraphicsDevice::find_device_surface_support(dev, surface);
		if (support) {
			support_per_dev.emplace_back(dev, *support);
		}
	}

	if (support_per_dev.empty()) {
		throw Error(MSG(err) << "No valid Vulkan device available.");
	}

	// TODO rate devices based on capabilities
	auto const& info = support_per_dev[0];

	// Create a logical device with a single queue for both graphics and present
	if (info.second.maybe_present_fam) {
		throw 0;
	}

	VlkGraphicsDevice dev(info.first, { info.second.graphics_fam } );

	VlkDrawableDisplay display(dev.get_device(), info.second);

	// TODO reinit swapchain on window resize
}

}}} // openage::renderer::vulkan
