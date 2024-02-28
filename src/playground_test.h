#pragma once

#include "fml/macros.h"
#include "gtest/gtest.h"
#include "vk.h"

namespace one::testing {

class PlaygroundTest : public ::testing::Test {
 public:
  PlaygroundTest();

  ~PlaygroundTest();

  PFN_vkGetInstanceProcAddr GetInstanceProcAddress() const;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(PlaygroundTest);
};

}  // namespace one::testing
