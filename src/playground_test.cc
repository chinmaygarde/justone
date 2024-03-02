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
  auto window = ::glfwCreateWindow(3000, 2000, "Just One", nullptr, nullptr);
  if (!window) {
    FML_LOG(ERROR) << "Unable to create glfw window";
    return;
  }
  window_.reset(window);
}

PlaygroundTest::~PlaygroundTest() = default;

PFN_vkGetInstanceProcAddr PlaygroundTest::GetInstanceProcAddress() const {
  return (PFN_vkGetInstanceProcAddr)::glfwGetInstanceProcAddress(
      nullptr, "vkGetInstanceProcAddr");
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
  }

  return false;
}

}  // namespace one::testing
