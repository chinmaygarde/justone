#pragma once

#include <set>
#include <string>

#include "fml/macros.h"
#include "vk.h"

namespace one {

class Caps {
 public:
  Caps();

  ~Caps();

  bool IsValid() const { return is_valid_; }

  bool HasValidationLayers() const { return has_validation_layers_; }

  bool HasExtension(const std::string& ext) const {
    return extensions_.contains(ext);
  }

 private:
  bool has_validation_layers_ = false;
  std::set<std::string> extensions_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Caps);
};

}  // namespace one
