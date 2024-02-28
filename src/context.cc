#include "context.h"

#include <memory>

#include "fml/logging.h"
#include "vk.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace one {

Context::Context(PFN_vkGetInstanceProcAddr proc_address_callback) {
  if (!proc_address_callback) {
    FML_LOG(ERROR) << "Invalid proc. address callback.";
    return;
  }
  VULKAN_HPP_DEFAULT_DISPATCHER.init(proc_address_callback);

  caps_ = std::make_unique<Caps>();
  if (!caps_->IsValid()) {
    return;
  }

  is_valid_ = true;
}

Context::~Context() = default;

bool Context::IsValid() const {
  return is_valid_;
}

}  // namespace one
