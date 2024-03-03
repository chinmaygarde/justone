#pragma once

#include "context.h"
#include "fml/macros.h"
#include "fml/unique_object.h"
#include "glm/glm/vec2.hpp"
#include "gtest/gtest.h"
#include "vk.h"

// Must be included after vk.h to enable the Vulkan defines.
#include "GLFW/glfw3.h"
#include "swapchain.h"
#include "vulkan/vulkan_handles.hpp"

namespace one::testing {

class PlaygroundTest : public ::testing::Test {
 public:
  PlaygroundTest();

  ~PlaygroundTest();

  bool OpenPlaygroundHere();

  bool IsValid() const { return is_valid_; }

 private:
  struct UniqueGLFWWindowTraits {
    static GLFWwindow* InvalidValue() { return nullptr; }

    static bool IsValid(const GLFWwindow* value) {
      return value != InvalidValue();
    }

    static void Free(GLFWwindow* window) { glfwDestroyWindow(window); }
  };

  fml::UniqueObject<GLFWwindow*, UniqueGLFWWindowTraits> window_;
  PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr_ = {};
  std::shared_ptr<Context> context_;
  std::unique_ptr<Swapchain> swapchain_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(PlaygroundTest);
};

}  // namespace one::testing
