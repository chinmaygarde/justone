#include "image_decoder.h"
#include <memory>
#include "fml/mapping.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace one {

ImageDecoder::ImageDecoder(const fml::Mapping& source) {
  int x = 0;
  int y = 0;
  int channels = 0;

  stbi_uc* decoded = ::stbi_load_from_memory(
      source.GetMapping(), source.GetSize(), &x, &y, &channels, STBI_rgb_alpha);

  if (decoded == nullptr || x < 0 || y < 0) {
    FML_LOG(ERROR) << "Could not load image data.";
    return;
  }

  decoded_ = std::make_unique<fml::NonOwnedMapping>(
      decoded,                 //
      x * y * STBI_rgb_alpha,  //
      [decoded](const uint8_t* data, size_t size) {
        ::stbi_image_free(decoded);
      });
  size_ = {x, y};
  is_valid_ = true;
}

ImageDecoder::~ImageDecoder() = default;

bool ImageDecoder::IsValid() const {
  return is_valid_;
}

glm::ivec2 ImageDecoder::GetSize() const {
  return size_;
}

}  // namespace one
