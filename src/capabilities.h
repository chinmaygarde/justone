#pragma once

#include <set>
#include <string>
#include <vector>

#include "fml/macros.h"
#include "vk.h"

namespace one {

class Capabilities {
 public:
  Capabilities();

  ~Capabilities();

  bool IsValid() const;

  bool HasExtension(const std::string& ext) const;

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
