#pragma once

#include <memory>

#include "fml/macros.h"
#include "fml/mapping.h"
#include "glm/glm/ext/vector_int2.hpp"

namespace one {

class ImageDecoder {
 public:
  ImageDecoder(const fml::Mapping& source);

  ~ImageDecoder();

  bool IsValid() const;

  glm::ivec2 GetSize() const;

 private:
  std::unique_ptr<fml::Mapping> decoded_;
  glm::ivec2 size_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(ImageDecoder);
};

}  // namespace one
