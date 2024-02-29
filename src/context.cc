#include "context.h"

#include <memory>

#include "fml/logging.h"
#include "vk.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "vulkan/vulkan_to_string.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace one {

Context::Context(PFN_vkGetInstanceProcAddr proc_address_callback) {
  if (!proc_address_callback) {
    FML_LOG(ERROR) << "Invalid proc. address callback.";
    return;
  }

  VULKAN_HPP_DEFAULT_DISPATCHER.init(proc_address_callback);

  caps_ = std::make_unique<Caps>();

  if (!caps_->IsValid()) {
    return;
  }

  vk::InstanceCreateFlags instance_flags;
  std::vector<const char*> exts;
  if (caps_->HasExtension("VK_KHR_portability_enumeration")) {
    exts.push_back("VK_KHR_portability_enumeration");
    instance_flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
  }

  vk::ApplicationInfo application_info;
  application_info.pApplicationName = "JustOne";
  application_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  application_info.pEngineName = "JustOne";
  application_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  application_info.apiVersion = VK_MAKE_API_VERSION(0, 1, 1, 0);

  vk::InstanceCreateInfo instance_info;
  instance_info.setPApplicationInfo(&application_info);
  instance_info.setPEnabledExtensionNames(exts);
  instance_info.flags = instance_flags;
  auto instance = vk::createInstance(instance_info);
  if (instance.result != vk::Result::eSuccess) {
    FML_LOG(ERROR) << "Could not create application: "
                   << vk::to_string(instance.result);
    return;
  }

  is_valid_ = true;
}

Context::~Context() = default;

bool Context::IsValid() const {
  return is_valid_;
}

}  // namespace one
