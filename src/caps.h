#pragma once

#include <set>
#include <string>
#include <vector>

#include "fml/macros.h"
#include "vk.h"
#include "vulkan/vulkan_handles.hpp"

namespace one {

class Capabilities {
 public:
  Capabilities();

  ~Capabilities();

  bool IsValid() const { return is_valid_; }

  bool HasValidationLayers() const { return has_validation_layers_; }

  bool HasExtension(const std::string& ext) const {
    return extensions_.contains(ext);
  }

  static void DumpToLog();

  static void DumpToLog(const vk::PhysicalDevice& device);

  static bool InstanceHasAllExtensions(
      const std::vector<std::string>& extensions);

  static bool DeviceHasAllExtensions(
      const vk::PhysicalDevice& device,
      const std::vector<std::string>& extensions);

 private:
  bool has_validation_layers_ = false;
  std::set<std::string> extensions_;
  bool is_valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(Capabilities);
};

}  // namespace one
