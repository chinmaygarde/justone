#pragma once

#include <memory>

#include "capabilities.h"
#include "fml/concurrent_message_loop.h"
#include "fml/macros.h"
#include "vk.h"

namespace one {

struct QueueIndexVK {
  uint32_t family = 0u;
  uint32_t index = 0u;
};

class Context final : public std::enable_shared_from_this<Context> {
 public:
  static std::shared_ptr<Context> Make(
      PFN_vkGetInstanceProcAddr proc_address_callback,
      const std::set<std::string>& additional_instance_extensions);

  ~Context();

  bool IsValid() const;

  const Capabilities& GetCapabilities() const;

  const vk::Instance& GetInstance() const;

  const vk::PhysicalDevice& GetPhysicalDevice() const;

  const vk::Device& GetDevice() const;

  const QueueIndexVK& GetQueueIndex() const;

  const vk::Queue& GetQueue() const;

  const std::shared_ptr<fml::ConcurrentTaskRunner>& GetConcurrentTaskRunner()
      const;

 private:
  std::unique_ptr<Capabilities> caps_;
  vk::UniqueInstance instance_;
  QueueIndexVK queue_index_;
  vk::PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
  vk::Queue queue_;
  std::shared_ptr<fml::ConcurrentMessageLoop> concurrent_message_loop_;
  std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner_;
  bool is_valid_ = false;

  Context(PFN_vkGetInstanceProcAddr proc_address_callback,
          const std::set<std::string>& additional_instance_extensions);

  FML_DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace one
