#include "context.h"
#include "gtest/gtest.h"
#include "playground_test.h"

namespace one::testing {

TEST_F(PlaygroundTest, CanCreateContext) {
  Context context(GetInstanceProcAddress());
  ASSERT_TRUE(context.IsValid());
}

}  // namespace one::testing
