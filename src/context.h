#pragma once

#include "macros.h"
#include "vulkan/vulkan.hpp"

namespace one {

class Context {
 public:
  Context();

  ~Context();

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace one
