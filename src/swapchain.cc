#include "swapchain.h"
#include <xatomic.h>

#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>
#include <vector>

#include "context.h"
#include "fml/logging.h"
#include "fml/macros.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace one {

class Swapchain::Synchronizer {
 public:
  Synchronizer(const vk::Device& device) {
    {
      vk::FenceCreateInfo fence_info;
      auto [result, acquire_fence] = device.createFenceUnique(fence_info);
      if (result != vk::Result::eSuccess) {
        return;
      }
      acquire_fence_ = std::move(acquire_fence);
    }

    {
      vk::SemaphoreCreateInfo sema_info;
      auto [result, present_wait_sema] =
          device.createSemaphoreUnique(sema_info);
      if (result != vk::Result::eSuccess) {
        return;
      }
      present_wait_sema_ = std::move(present_wait_sema);
    }

    is_valid_ = true;
  }

  bool IsValid() const { return is_valid_; }

  const vk::Fence& GetAcquireFence() const { return *acquire_fence_; }

  const vk::Semaphore& GetPresentWaitSemaphore() const {
    return *present_wait_sema_;
  }

  bool WaitAndResetAcquireFence() const {
    using namespace std::chrono_literals;
    constexpr auto timeout_ns = std::chrono::nanoseconds(10s).count();
    const auto& device = acquire_fence_.getOwner();
    if (device.waitForFences(*acquire_fence_, true, timeout_ns) !=
        vk::Result::eSuccess) {
      return false;
    }
    if (device.resetFences(*acquire_fence_) != vk::Result::eSuccess) {
      return false;
    }
    return true;
  }

 private:
  vk::UniqueFence acquire_fence_;
  vk::UniqueSemaphore present_wait_sema_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Synchronizer);
};

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

  static constexpr const auto kSwapchainImageUsage =
      vk::ImageUsageFlagBits::eTransferDst |
      vk::ImageUsageFlagBits::eColorAttachment |
      vk::ImageUsageFlagBits::eInputAttachment;

  if (!(surface_caps.supportedUsageFlags & kSwapchainImageUsage)) {
    return;
  }

  vk::SwapchainCreateInfoKHR swapchain_info;

  swapchain_info.flags = {};
  swapchain_info.surface = surface_.get();
  swapchain_info.minImageCount = std::clamp(
      surface_caps.minImageCount + 1, surface_caps.minImageCount,
      surface_caps.maxImageCount == 0u ? surface_caps.minImageCount + 1u
                                       : surface_caps.maxImageCount);
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

  auto [swapchain_result, swapchain] =
      context->GetDevice().createSwapchainKHRUnique(swapchain_info);
  if (swapchain_result != vk::Result::eSuccess) {
    return;
  }
  swapchain_ = std::move(swapchain);

  auto [images_result, images] =
      context->GetDevice().getSwapchainImagesKHR(*swapchain_);
  if (images_result != vk::Result::eSuccess) {
    return;
  }
  images_ = std::move(images);

  for (const auto& image : images_) {
    synchronizers_.emplace_back(
        std::make_unique<Synchronizer>(context->GetDevice()));
    if (!synchronizers_.back()->IsValid()) {
      return;
    }
  }

  FML_CHECK(synchronizers_.size() == images_.size());

  is_valid_ = true;
}

Swapchain::~Swapchain() = default;

bool Swapchain::IsValid() const {
  return is_valid_;
}

bool Swapchain::Render() {
  auto context = context_.lock();
  if (!context) {
    return false;
  }

  const auto& device = context->GetDevice();

  frame_count_++;

  const auto& sync = synchronizers_.at(frame_count_ % synchronizers_.size());

  using namespace std::chrono_literals;
  static constexpr auto kTimeoutNS = std::chrono::nanoseconds(10s);

  const auto [acquire_result, index] = device.acquireNextImageKHR(
      *swapchain_, kTimeoutNS.count(), {}, sync->GetAcquireFence());
  if (acquire_result != vk::Result::eSuccess) {
    return false;
  }

  if (!sync->WaitAndResetAcquireFence()) {
    return false;
  }

  // Do the rendering here.

  // Done rendering. Submit a signal to the semaphore the presentation engine is
  // going to wait on before presenting.
  {
    vk::SubmitInfo submit_info;
    submit_info.setSignalSemaphores(sync->GetPresentWaitSemaphore());
    if (context->GetQueue().submit(submit_info) != vk::Result::eSuccess) {
      return false;
    }
  }

  {
    vk::PresentInfoKHR present_info;
    present_info.setWaitSemaphores(sync->GetPresentWaitSemaphore());
    present_info.setSwapchains(*swapchain_);
    present_info.setImageIndices(index);
    if (context->GetQueue().presentKHR(present_info) != vk::Result::eSuccess) {
      return false;
    }
  }

  return false;
}

}  // namespace one
