#include "capabilities.h"

#include <algorithm>

#include "fml/logging.h"
#include "vk.h"

namespace one {

Capabilities::Capabilities() {
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

Capabilities::~Capabilities() = default;

void Capabilities::DumpToLog() {
  if (auto layers = vk::enumerateInstanceLayerProperties();
      layers.result == vk::Result::eSuccess) {
    FML_LOG(IMPORTANT) << "Found " << layers.value.size() << " layers.";
    for (const auto& layer : layers.value) {
      FML_LOG(IMPORTANT) << layer.layerName;
    }
  }
  if (auto exts = vk::enumerateInstanceExtensionProperties();
      exts.result == vk::Result::eSuccess) {
    FML_LOG(IMPORTANT) << "Found " << exts.value.size() << " extensions.";
    for (const auto& ext : exts.value) {
      FML_LOG(IMPORTANT) << ext.extensionName << "(" << ext.specVersion << ")";
    }
  }
}

void Capabilities::DumpToLog(const vk::PhysicalDevice& device) {
  const auto props = device.getProperties2();
  FML_LOG(IMPORTANT) << "Device Name: " << props.properties.deviceName;
  if (auto exts = device.enumerateDeviceExtensionProperties();
      exts.result == vk::Result::eSuccess) {
    FML_LOG(IMPORTANT) << "Found " << exts.value.size() << " extensions.";
    for (const auto& ext : exts.value) {
      FML_LOG(IMPORTANT) << ext.extensionName << "(" << ext.specVersion << ")";
    }
  }
}

bool Capabilities::InstanceHasAllExtensions(
    const std::vector<std::string>& extensions) {
  std::set<std::string> supported_extensions;
  for (const auto& ext : vk::enumerateInstanceExtensionProperties().value) {
    supported_extensions.insert(ext.extensionName);
  }
  for (const auto& ext : extensions) {
    if (!supported_extensions.contains(ext)) {
      FML_LOG(ERROR) << "Device doesn't support extension: " << ext;
      return false;
    }
  }
  return true;
}

bool Capabilities::DeviceHasAllExtensions(
    const vk::PhysicalDevice& device,
    const std::vector<std::string>& extensions) {
  std::set<std::string> supported_extensions;
  for (const auto& ext : device.enumerateDeviceExtensionProperties().value) {
    supported_extensions.insert(ext.extensionName);
  }

  for (const auto& ext : extensions) {
    if (!supported_extensions.contains(ext)) {
      FML_LOG(ERROR) << "Device doesn't support extension: " << ext;
      return false;
    }
  }

  return true;
}

bool Capabilities::IsValid() const {
  return is_valid_;
}

bool Capabilities::HasExtension(const std::string& ext) const {
  return extensions_.contains(ext);
}

}  // namespace one
