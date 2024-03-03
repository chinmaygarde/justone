#include "assets_location.h"
#include "context.h"
#include "fml/mapping.h"
#include "gtest/gtest.h"
#include "image_decoder.h"
#include "playground_test.h"

namespace one::testing {

TEST(JustOne, CanDecodeImage) {
  auto airplane =
      fml::FileMapping::CreateReadOnly(JUSTONE_ASSETS_LOCATION "airplane.jpg");
  ASSERT_TRUE(airplane && airplane->IsValid());
  ImageDecoder decoder(*airplane);
  EXPECT_TRUE(decoder.IsValid());
  EXPECT_EQ(decoder.GetSize().x, 487u);
  EXPECT_EQ(decoder.GetSize().y, 378u);
}

TEST_F(PlaygroundTest, CanShowWindow) {
  ASSERT_TRUE(OpenPlaygroundHere());
}

}  // namespace one::testing
