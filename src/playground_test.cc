
#include "playground_test.h"

#include <mutex>

#include "GLFW/glfw3.h"
#include "fml/logging.h"

namespace one::testing {

static void InitGLFWOnce() {
  static std::once_flag gOnce;
  std::call_once(gOnce, []() { FML_CHECK(::glfwInit() == GLFW_TRUE); });
}

PlaygroundTest::PlaygroundTest() {
  InitGLFWOnce();
  FML_CHECK(::glfwVulkanSupported())
      << "Vulkan must be supported on this platform";
  ::glfwDefaultWindowHints();
  ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  auto window = ::glfwCreateWindow(1280, 800, "Just One", nullptr, nullptr);
  if (!window) {
    FML_LOG(ERROR) << "Unable to create glfw window";
    return;
  }
  int width = 0;
  int height = 0;
  ::glfwGetWindowSize(window, &width, &height);
  window_size_ = {width, height};
}

PlaygroundTest::~PlaygroundTest() = default;

PFN_vkGetInstanceProcAddr PlaygroundTest::GetInstanceProcAddress() const {
  return (PFN_vkGetInstanceProcAddr)::glfwGetInstanceProcAddress(
      NULL, "vkGetInstanceProcAddr");
}

}  // namespace one::testing
