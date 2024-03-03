#pragma once

#include <memory>

#include "capabilities.h"
#include "fml/macros.h"
#include "vk.h"
#include "vulkan/vulkan_handles.hpp"

namespace one {

struct QueueIndexVK {
  size_t family = 0u;
  size_t index = 0u;
};

class Context {
 public:
  Context(PFN_vkGetInstanceProcAddr proc_address_callback,
          const std::set<std::string>& additional_instance_extensions);

  ~Context();

  bool IsValid() const;

  const Capabilities& GetCapabilities() const { return *caps_; }

  const vk::Instance& GetInstance() const;

 private:
  std::unique_ptr<Capabilities> caps_;
  vk::UniqueInstance instance_;
  QueueIndexVK queue_index_;
  vk::PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace one
