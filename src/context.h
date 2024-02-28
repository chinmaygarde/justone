#pragma once

#include <memory>
#include "caps.h"
#include "fml/macros.h"
#include "macros.h"
#include "vk.h"

namespace one {

class Context {
 public:
  Context(PFN_vkGetInstanceProcAddr proc_address_callback);

  ~Context();

  bool IsValid() const;

 private:
  std::unique_ptr<Caps> caps_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace one
