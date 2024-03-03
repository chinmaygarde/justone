#include "swapchain.h"

#include <iterator>
#include <optional>
#include <vector>

#include "context.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace one {

static std::optional<vk::SurfaceFormatKHR> PickSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& formats) {
  for (const auto& format : formats) {
    switch (format.format) {
      case vk::Format::eB8G8R8A8Unorm:
      case vk::Format::eR8G8B8A8Unorm:
        return format;
      default:
        break;
    }
  }
  return std::nullopt;
}

constexpr vk::SurfaceTransformFlagBitsKHR Pick(
    vk::SurfaceTransformFlagsKHR supported) {
  if (supported & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
    return vk::SurfaceTransformFlagBitsKHR::eIdentity;
  }

  if (supported & vk::SurfaceTransformFlagBitsKHR::eInherit) {
    return vk::SurfaceTransformFlagBitsKHR::eInherit;
  }

  return vk::SurfaceTransformFlagBitsKHR::eInherit;
}

constexpr vk::CompositeAlphaFlagBitsKHR Pick(
    vk::CompositeAlphaFlagsKHR supported) {
  if (supported & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
    return vk::CompositeAlphaFlagBitsKHR::eOpaque;
  }

  if (supported & vk::CompositeAlphaFlagBitsKHR::eInherit) {
    return vk::CompositeAlphaFlagBitsKHR::eInherit;
  }

  return vk::CompositeAlphaFlagBitsKHR::eInherit;
}

static vk::PresentModeKHR Pick(
    const std::vector<vk::PresentModeKHR>& supported_vec) {
  std::set<vk::PresentModeKHR> supported;
  for (const auto& mode : supported_vec) {
    supported.insert(mode);
  }
  if (supported.contains(vk::PresentModeKHR::eMailbox)) {
    return vk::PresentModeKHR::eMailbox;
  }
  if (supported.contains(VULKAN_HPP_NAMESPACE::PresentModeKHR::eFifo)) {
    return vk::PresentModeKHR::eFifo;
  }
  return vk::PresentModeKHR::eFifo;
}

Swapchain::Swapchain(const std::shared_ptr<Context>& context,
                     vk::UniqueSurfaceKHR p_surface)
    : surface_(std::move(p_surface)), context_(context) {
  const auto [surface_caps_result, surface_caps] =
      context->GetPhysicalDevice().getSurfaceCapabilitiesKHR(*surface_);
  if (surface_caps_result != vk::Result::eSuccess) {
    return;
  }

  const auto [surface_formats_result, surface_formats] =
      context->GetPhysicalDevice().getSurfaceFormatsKHR(*surface_);
  if (surface_caps_result != vk::Result::eSuccess) {
    return;
  }

  const auto [surface_present_modes_result, surface_present_modes] =
      context->GetPhysicalDevice().getSurfacePresentModesKHR(*surface_);
  if (surface_formats_result != vk::Result::eSuccess) {
    return;
  }

  const auto surface_format = PickSurfaceFormat(surface_formats);
  if (!surface_format.has_value()) {
    return;
  }

  vk::SwapchainCreateInfoKHR swapchain_info;

  static constexpr const auto kSwapchainImageUsage =
      vk::ImageUsageFlagBits::eTransferDst |
      vk::ImageUsageFlagBits::eColorAttachment |
      vk::ImageUsageFlagBits::eInputAttachment;

  if (!(surface_caps.supportedUsageFlags & kSwapchainImageUsage)) {
    return;
  }

  swapchain_info.flags = {};
  swapchain_info.surface = surface_.get();
  swapchain_info.minImageCount = surface_caps.minImageCount;
  swapchain_info.imageFormat = surface_format->format;
  swapchain_info.imageColorSpace = surface_format->colorSpace;
  swapchain_info.imageExtent = surface_caps.currentExtent;
  swapchain_info.imageArrayLayers = 1u;
  swapchain_info.imageUsage = kSwapchainImageUsage;
  swapchain_info.imageSharingMode = vk::SharingMode::eExclusive;
  swapchain_info.preTransform = Pick(surface_caps.supportedTransforms);
  swapchain_info.compositeAlpha = Pick(surface_caps.supportedCompositeAlpha);
  swapchain_info.presentMode = Pick(surface_present_modes);
  swapchain_info.clipped = false;
  swapchain_info.oldSwapchain = nullptr;

  const auto [swapchain_result, swapchain_] =
      context->GetDevice().createSwapchainKHRUnique(swapchain_info);
  if (swapchain_result != vk::Result::eSuccess) {
    return;
  }

  is_valid_ = true;
}

Swapchain::~Swapchain() = default;

}  // namespace one
