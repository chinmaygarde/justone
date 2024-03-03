#pragma once

#include <chrono>
#include <memory>

#include "fml/macros.h"
#include "vk.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace one {

class Context;

class Swapchain {
 public:
  Swapchain(const std::shared_ptr<Context>& context,
            vk::UniqueSurfaceKHR surface);

  ~Swapchain();

  bool IsValid() const;

  bool Render();

 private:
  class Synchronizer;

  std::weak_ptr<Context> context_;
  vk::UniqueSurfaceKHR surface_;
  vk::UniqueSwapchainKHR swapchain_;
  size_t frame_count_ = 0u;
  std::vector<std::unique_ptr<Synchronizer>> synchronizers_;
  std::vector<vk::Image> images_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Swapchain);
};

}  // namespace one
