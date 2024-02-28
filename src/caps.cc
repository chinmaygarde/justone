#include "caps.h"
#include <algorithm>
#include "vulkan/vulkan_enums.hpp"

namespace one {

Caps::Caps() {
  auto layers = vk::enumerateInstanceLayerProperties();
  if (layers.result != vk::Result::eSuccess) {
    return;
  }
  has_validation_layers_ =
      std::find_if(layers.value.cbegin(), layers.value.cend(),
                   [](const auto& layer) {
                     return layer.layerName == "VK_LAYER_KHRONOS_validation";
                   }) != layers.value.end();
  is_valid_ = true;
}

Caps::~Caps() {}

}  // namespace one
