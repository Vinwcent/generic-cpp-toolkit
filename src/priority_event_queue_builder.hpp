#ifndef VVW_PRIORITY_EVENT_QUEUE_BUILDER_HPP
#define VVW_PRIORITY_EVENT_QUEUE_BUILDER_HPP

#include "src/function_wrappers.hpp"
#include "src/macros.hpp"
#include "src/priority_event_queue.hpp"

BEGIN_VVW_GEN_LIB_NS

template <EnumClass Event>
class PriorityEventQueueBuilder {
 public:
  template <typename... Args>
  void registerEvent(Event event, int priority,
                     std::function<void(Args...)> eventFunction) {
    queueConfig_.eventPriorities[event] = priority;
    queueConfig_.eventsFunctions[event] =
        std::make_unique<FunctionWrapper<Args...>>(eventFunction);
  }

  std::unique_ptr<PriorityEventQueue<Event>> build() {
    auto eventQueue =
        std::make_unique<PriorityEventQueue<Event>>(std::move(queueConfig_));
    queueConfig_ = PriorityEventQueueConfig<Event>();
    return eventQueue;
  }

 private:
  PriorityEventQueueConfig<Event> queueConfig_;
};

END_VVW_GEN_LIB_NS

#endif
