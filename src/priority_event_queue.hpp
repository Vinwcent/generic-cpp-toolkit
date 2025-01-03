#ifndef VVW_PRIORITY_EVENT_QUEUE_HPP
#define VVW_PRIORITY_EVENT_QUEUE_HPP

#include <map>
#include <mutex>
#include <set>

#include "macros.hpp"
#include "src/args_storage.hpp"
#include "src/function_wrappers.hpp"
#include "src/worker.hpp"

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
    if (!worker) {
      throw std::runtime_error(
          "Worker must be initialized before adding events");
    }
    std::lock_guard<std::mutex> lock(worker->getMutex());
    eventToArgs_[event].push_back(
        std::make_unique<ArgsStorage<Args...>>(args...));
    worker->notifyWorkWasAdded();
  }

  int getNPendingEvents(Event event) {
    if (!worker) {
      throw std::runtime_error(
          "Worker must be initialized before getting the number of pending "
          "events");
    }
    std::lock_guard<std::mutex> lock(worker->getMutex());
    return eventToArgs_[event].size();
  }

  void startProcessingEvents() {
    worker = std::make_unique<Worker>(
        [this](std::unique_lock<std::mutex>& lock) {
          this->processNextEvent_(lock);
        },
        [this]() { return this->hasEventsToProcess_(); });
  }

 private:
  std::vector<Event> orderedEvents_{};
  std::map<Event, std::unique_ptr<FunctionWrapperTypeEraser>>
      eventsFunctions_{};
  std::map<Event, std::vector<std::unique_ptr<ArgsStorageTypeEraser>>>
      eventToArgs_{};

  std::unique_ptr<Worker> worker;

  void processNextEvent_(std::unique_lock<std::mutex>& lock) {
    for (auto event : orderedEvents_) {
      int nArgsToProcess = eventToArgs_[event].size();
      if (nArgsToProcess < 1) {
        continue;
      }
      std::unique_ptr<ArgsStorageTypeEraser> argsStorage =
          std::move(eventToArgs_[event][0]);
      eventToArgs_[event].erase(eventToArgs_[event].begin());
      lock.unlock();
      std::vector<void*> args = argsStorage->getArgPtrs();
      (*eventsFunctions_[event])(args);
      lock.lock();
      break;
    }
  }

  bool hasEventsToProcess_() {
    for (auto event : orderedEvents_) {
      int nArgsToProcess = eventToArgs_[event].size();
      if (nArgsToProcess > 0) {
        return true;
      }
    }
    return false;
  }

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
