#pragma once

#include "fml/macros.h"
#include "vk.h"

namespace one {

class Caps {
 public:
  Caps();

  ~Caps();

  bool IsValid() const { return is_valid_; }

  bool HasValidationLayers() const { return has_validation_layers_; }

 private:
  bool has_validation_layers_ = false;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Caps);
};

}  // namespace one
