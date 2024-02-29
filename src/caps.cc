#include "caps.h"
#include <algorithm>
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"

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

  auto exts = vk::enumerateInstanceExtensionProperties();
  if (exts.result != vk::Result::eSuccess) {
    return;
  }
  for (const auto& ext : exts.value) {
    extensions_.insert(ext.extensionName);
  }
  is_valid_ = true;
}

Caps::~Caps() = default;

}  // namespace one
