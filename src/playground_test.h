#pragma once

#include "fml/macros.h"
#include "fml/unique_object.h"
#include "glm/glm/vec2.hpp"
#include "gtest/gtest.h"
#include "vk.h"

// Must be included after vk.h to enable the Vulkan defines.
#include "GLFW/glfw3.h"

namespace one::testing {

class PlaygroundTest : public ::testing::Test {
 public:
  PlaygroundTest();

  ~PlaygroundTest();

  PFN_vkGetInstanceProcAddr GetInstanceProcAddress() const;

  bool OpenPlaygroundHere();

  bool IsValid() const { return window_.is_valid(); }

 private:
  struct UniqueGLFWWindowTraits {
    static GLFWwindow* InvalidValue() { return nullptr; }

    static bool IsValid(const GLFWwindow* value) {
      return value != InvalidValue();
    }

    static void Free(GLFWwindow* window) { glfwDestroyWindow(window); }
  };

  fml::UniqueObject<GLFWwindow*, UniqueGLFWWindowTraits> window_;
  FML_DISALLOW_COPY_AND_ASSIGN(PlaygroundTest);
};

}  // namespace one::testing
