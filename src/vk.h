#pragma once

#define VK_NO_PROTOTYPES
#define VULKAN_HPP_ASSERT FML_CHECK
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT(ignored) \
  { [[maybe_unused]] auto res = (ignored); }

#include "fml/logging.h"
#include "vulkan/vulkan.hpp"

namespace one {

constexpr bool IsSuccess(vk::Result result) {
  return result == vk::Result::eSuccess;
}

}  // namespace one
