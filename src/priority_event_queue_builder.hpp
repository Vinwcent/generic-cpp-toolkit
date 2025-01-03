#ifndef VVW_PRIORITY_EVENT_QUEUE_BUILDER_HPP
#define VVW_PRIORITY_EVENT_QUEUE_BUILDER_HPP

#include "src/function_wrappers.hpp"
#include "src/macros.hpp"
#include "src/priority_event_queue.hpp"

BEGIN_VVW_GEN_LIB_NS

template <EnumClass Event>
class PriorityEventQueueBuilder {
 public:
  void registerEvent(Event event, int priority,
                     FunctionWrapperBase eventFunction) {}

 private:
  PriorityEventQueueConfig<Event> queueConfig_;
};

END_VVW_GEN_LIB_NS

#endif
