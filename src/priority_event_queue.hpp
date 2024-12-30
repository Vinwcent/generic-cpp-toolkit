#ifndef VVW_PRIORITY_EVENT_QUEUE_HPP
#define VVW_PRIORITY_EVENT_QUEUE_HPP

#include <concepts>
#include <map>
#include <set>

#include "macros.hpp"
#include "src/args_storage.hpp"
#include "src/function_wrappers.hpp"

BEGIN_VVW_GEN_LIB_NS

template <typename T>
concept EnumClass = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

template <EnumClass Event>
struct PriorityEventQueueConfig {
  std::map<Event, int> eventPriorities{};
  std::map<Event, std::unique_ptr<FunctionWrapperTypeEraser>> eventsFunctions{};
};

template <EnumClass Event>
class PriorityEventQueue {
 public:
  PriorityEventQueue(PriorityEventQueueConfig<Event>&& config) {
    checkValidity_(config);
    initOrderedEvents_(config.eventPriorities);
    eventsFunctions_ = std::move(config.eventsFunctions);
    for (auto& [event, _] : config.eventPriorities) {
      eventToArgs_[event].clear();
    }
  }

  template <typename... Args>
  void addEvent(Event event, Args... args) {
    eventToArgs_[event].push_back(
        std::make_unique<ArgsStorage<Args...>>(args...));
  }

  int getNPendingEvents(Event event) { return eventToArgs_[event].size(); }

 private:
  std::vector<Event> orderedEvents_{};
  std::map<Event, std::unique_ptr<FunctionWrapperTypeEraser>>
      eventsFunctions_{};
  std::map<Event, std::vector<std::unique_ptr<ArgsStorageTypeEraser>>>
      eventToArgs_{};

  void checkValidity_(const PriorityEventQueueConfig<Event>& config) {
    std::set<Event> events;
    std::set<int> priorities;

    for (const auto& [event, priority] : config.eventPriorities) {
      if (!events.insert(event).second) {
        throw std::invalid_argument("Duplicate event detected");
      }
      if (!priorities.insert(priority).second) {
        throw std::invalid_argument("Duplicate priority detected");
      }
    }
  }

  void initOrderedEvents_(std::map<Event, int>& eventPriorities) {
    std::vector<std::pair<Event, int>> sorted_pairs(eventPriorities.begin(),
                                                    eventPriorities.end());
    std::sort(sorted_pairs.begin(), sorted_pairs.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    orderedEvents_.reserve(sorted_pairs.size());
    for (const auto& [event, _] : sorted_pairs) {
      orderedEvents_.push_back(event);
    }
  }
};

END_VVW_GEN_LIB_NS
#endif
