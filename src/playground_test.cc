#include "playground_test.h"

#include <memory>
#include <mutex>

#include "GLFW/glfw3.h"
#include "context.h"
#include "fml/logging.h"
#include "swapchain.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"

namespace one::testing {

static void InitGLFWOnce() {
  static std::once_flag gOnce;
  std::call_once(gOnce, []() { FML_CHECK(::glfwInit() == GLFW_TRUE); });
}

static std::set<std::string> GetAdditionalRequiredInstanceExtensions() {
  std::set<std::string> exts;
  uint32_t count = 0u;
  auto extensions = glfwGetRequiredInstanceExtensions(&count);
  for (size_t i = 0; i < count; i++) {
    exts.insert(extensions[i]);
  }
  return exts;
}

PlaygroundTest::PlaygroundTest() {
  InitGLFWOnce();
  FML_CHECK(::glfwVulkanSupported())
      << "Vulkan must be supported on this platform";
  ::glfwDefaultWindowHints();
  ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  auto window = ::glfwCreateWindow(3000, 2000, "Just One", nullptr, nullptr);
  if (!window) {
    FML_LOG(ERROR) << "Unable to create glfw window";
    return;
  }
  window_.reset(window);

  vk_get_instance_proc_addr_ =
      (PFN_vkGetInstanceProcAddr)::glfwGetInstanceProcAddress(
          nullptr, "vkGetInstanceProcAddr");

  context_ = Context::Make(vk_get_instance_proc_addr_,
                           GetAdditionalRequiredInstanceExtensions());
  if (!context_) {
    return;
  }

  VkSurfaceKHR surface = {};
  if (const auto result = ::glfwCreateWindowSurface(
          context_->GetInstance(), window_.get(), nullptr, &surface);
      result != VK_SUCCESS) {
    FML_LOG(ERROR) << "Could not create surface: "
                   << vk::to_string(vk::Result(result));
    return;
  }

  swapchain_ = std::make_unique<Swapchain>(
      context_, vk::UniqueSurfaceKHR{surface, context_->GetInstance()});

  if (!swapchain_ || !swapchain_->IsValid()) {
    return;
  }

  is_valid_ = true;
}

PlaygroundTest::~PlaygroundTest() = default;

bool PlaygroundTest::IsValid() const {
  return is_valid_;
}

static void PlaygroundKeyCallback(GLFWwindow* window,
                                  int key,
                                  int scancode,
                                  int action,
                                  int mods) {
  if ((key == GLFW_KEY_ESCAPE) && action == GLFW_RELEASE) {
    ::glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

bool PlaygroundTest::OpenPlaygroundHere() {
  if (!IsValid()) {
    return false;
  }

  ::glfwSetWindowTitle(window_.get(), "JustOne Playground (Press ESC to quit)");
  ::glfwSetWindowUserPointer(window_.get(), this);
  ::glfwSetKeyCallback(window_.get(), &PlaygroundKeyCallback);

  while (true) {
    ::glfwPollEvents();
    if (::glfwWindowShouldClose(window_.get())) {
      return true;
    }
    if (!swapchain_->Render()) {
      return false;
    }
  }

  return false;
}

}  // namespace one::testing
