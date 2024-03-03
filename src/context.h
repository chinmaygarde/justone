#pragma once

#include <memory>

#include "capabilities.h"
#include "fml/macros.h"
#include "vk.h"
#include "vulkan/vulkan_handles.hpp"

namespace one {

struct QueueIndexVK {
  uint32_t family = 0u;
  uint32_t index = 0u;
};

class Context final : public std::enable_shared_from_this<Context> {
 public:
  static std::shared_ptr<Context> Make(
      PFN_vkGetInstanceProcAddr proc_address_callback,
      const std::set<std::string>& additional_instance_extensions);

  ~Context();

  bool IsValid() const;

  const Capabilities& GetCapabilities() const { return *caps_; }

  const vk::Instance& GetInstance() const;

  const vk::PhysicalDevice& GetPhysicalDevice() const {
    return physical_device_;
  }

  const vk::Device& GetDevice() const { return *device_; }

 private:
  std::unique_ptr<Capabilities> caps_;
  vk::UniqueInstance instance_;
  QueueIndexVK queue_index_;
  vk::PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
  bool is_valid_ = false;

  Context(PFN_vkGetInstanceProcAddr proc_address_callback,
          const std::set<std::string>& additional_instance_extensions);

  FML_DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace one
