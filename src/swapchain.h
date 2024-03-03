#pragma once

#include <memory>

#include "fml/macros.h"
#include "vk.h"
#include "vulkan/vulkan_handles.hpp"

namespace one {

class Context;

class Swapchain {
 public:
  Swapchain(const std::shared_ptr<Context>& context,
            vk::UniqueSurfaceKHR surface);

  ~Swapchain();

  bool IsValid() const;

 private:
  std::weak_ptr<Context> context_;
  vk::UniqueSurfaceKHR surface_;
  vk::UniqueSwapchainKHR swapchain_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Swapchain);
};

}  // namespace one
