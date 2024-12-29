#ifndef VVW_PRIORITY_EVENT_QUEUE_HPP
#define VVW_PRIORITY_EVENT_QUEUE_HPP

#include <concepts>
#include <map>
#include <set>

#include "macros.hpp"
#include "src/function_wrappers.hpp"

BEGIN_VVW_GEN_LIB_NS

template <typename T>
concept EnumClass = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

template <EnumClass Event>
class PriorityEventQueueConfig {
  std::map<Event, int> eventsPriority{};
};

template <EnumClass Event>
class PriorityEventQueue {
 public:
  PriorityEventQueue(PriorityEventQueueConfig<Event> config) {
    checkValidity_(config);
  }

 private:
  std::map<Event, FunctionWrapperTypeEraser> eventFunctions_;

  void checkValidity_(const PriorityEventQueueConfig<Event>& config) {
    std::set<Event> events;
    std::set<int> priorities;

    for (const auto& [event, priority] : config.eventsPriority) {
      if (!events.insert(event).second) {
        throw std::invalid_argument("Duplicate event detected");
      }
      if (!priorities.insert(priority).second) {
        throw std::invalid_argument("Duplicate priority detected");
      }
    }
  }
};

END_VVW_GEN_LIB_NS
#endif
