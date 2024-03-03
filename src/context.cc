#include "context.h"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <queue>

#include "capabilities.h"
#include "fml/logging.h"
#include "vk.h"
#include "vulkan/vulkan_handles.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace one {

static const std::vector<std::string> kRequiredDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static constexpr auto kAllCapabilitiesQueue = vk::QueueFlagBits::eGraphics |
                                              vk::QueueFlagBits::eCompute |
                                              vk::QueueFlagBits::eTransfer;

static std::optional<QueueIndexVK> PickQueue(const vk::PhysicalDevice& device,
                                             vk::QueueFlags flags) {
  // This can be modified to ensure that dedicated queues are returned for each
  // queue type depending on support.
  const auto families = device.getQueueFamilyProperties();
  for (uint32_t i = 0u; i < families.size(); i++) {
    if (!(families[i].queueFlags & flags)) {
      continue;
    }
    return QueueIndexVK{.family = i, .index = 0};
  }
  return std::nullopt;
}

static vk::PhysicalDevice PickPhysicalDevice(const vk::Instance& instance) {
  auto physical_devices = instance.enumeratePhysicalDevices();
  if (physical_devices.result != vk::Result::eSuccess) {
    return {};
  }

  for (const auto& physical_device : physical_devices.value) {
    if (!Capabilities::DeviceHasAllExtensions(physical_device,
                                              kRequiredDeviceExtensions)) {
      return {};
    }
    if (!PickQueue(physical_device, kAllCapabilitiesQueue).has_value()) {
      return {};
    }
    return physical_device;
  }
  return {};
}

static vk::UniqueDevice CreateDevice(const vk::PhysicalDevice& device,
                                     const QueueIndexVK& queue_index) {
  vk::DeviceCreateInfo device_info;

  std::vector<const char*> required_extensions;
  required_extensions.reserve(kRequiredDeviceExtensions.size());
  for (const auto& ext : kRequiredDeviceExtensions) {
    required_extensions.push_back(ext.c_str());
  }
  device_info.setPEnabledExtensionNames(required_extensions);

  std::array<float, 1u> queue_priorities = {1.0f};
  vk::DeviceQueueCreateInfo queue_info;
  queue_info.flags = {};
  queue_info.queueFamilyIndex = queue_index.family;
  queue_info.queueCount = queue_priorities.size();
  queue_info.pQueuePriorities = queue_priorities.data();

  device_info.setQueueCreateInfos(queue_info);

  vk::PhysicalDeviceFeatures device_features;

  device_info.setPEnabledFeatures(&device_features);

  return device.createDeviceUnique(device_info).value;
}

std::shared_ptr<Context> Context::Make(
    PFN_vkGetInstanceProcAddr proc_address_callback,
    const std::set<std::string>& additional_instance_extensions) {
  auto context = std::shared_ptr<Context>(
      new Context(proc_address_callback, additional_instance_extensions));
  if (!context->IsValid()) {
    return nullptr;
  }
  return context;
}

Context::Context(PFN_vkGetInstanceProcAddr proc_address_callback,
                 const std::set<std::string>& additional_instance_extensions) {
  if (!proc_address_callback) {
    FML_LOG(ERROR) << "Invalid proc. address callback.";
    return;
  }

  VULKAN_HPP_DEFAULT_DISPATCHER.init(proc_address_callback);

  caps_ = std::make_unique<Capabilities>();

  if (!caps_->IsValid()) {
    return;
  }

  vk::InstanceCreateFlags instance_flags;
  std::vector<const char*> exts;
  if (caps_->HasExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
    exts.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instance_flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
  }
  if (!caps_->HasExtension(VK_KHR_SURFACE_EXTENSION_NAME)) {
    return;
  } else {
    exts.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  }

  for (const auto& additional_ext : additional_instance_extensions) {
    if (!caps_->HasExtension(additional_ext)) {
      FML_LOG(ERROR) << "Additional extension not supported: "
                     << additional_ext;
      return;
    } else {
      exts.push_back(additional_ext.c_str());
    }
  }

  vk::ApplicationInfo application_info;
  application_info.pApplicationName = "JustOne";
  application_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  application_info.pEngineName = "JustOne";
  application_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  application_info.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);

  vk::InstanceCreateInfo instance_info;
  instance_info.setPApplicationInfo(&application_info);
  instance_info.setPEnabledExtensionNames(exts);
  instance_info.flags = instance_flags;
  auto instance = vk::createInstanceUnique(instance_info);
  if (instance.result != vk::Result::eSuccess) {
    return;
  }
  instance_ = std::move(instance.value);

  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);

  auto physical_device = PickPhysicalDevice(*instance_);
  if (!physical_device) {
    return;
  }
  physical_device_ = physical_device;

  auto queue_index = PickQueue(physical_device, kAllCapabilitiesQueue);
  if (!queue_index.has_value()) {
    return;
  }
  queue_index_ = queue_index.value();

  device_ = CreateDevice(physical_device_, queue_index_);
  if (!device_) {
    return;
  }

  is_valid_ = true;
}

Context::~Context() = default;

bool Context::IsValid() const {
  return is_valid_;
}

const vk::Instance& Context::GetInstance() const {
  return *instance_;
}

const vk::PhysicalDevice& Context::GetPhysicalDevice() const {
  return physical_device_;
}

const Capabilities& Context::GetCapabilities() const {
  return *caps_;
}

const vk::Device& Context::GetDevice() const {
  return *device_;
}

}  // namespace one
