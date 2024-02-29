#pragma once

#include "fml/macros.h"
#include "glm/glm/vec2.hpp"
#include "gtest/gtest.h"
#include "vk.h"

namespace one::testing {

class PlaygroundTest : public ::testing::Test {
 public:
  PlaygroundTest();

  ~PlaygroundTest();

  PFN_vkGetInstanceProcAddr GetInstanceProcAddress() const;

 private:
  glm::ivec2 window_size_;
  FML_DISALLOW_COPY_AND_ASSIGN(PlaygroundTest);
};

}  // namespace one::testing
