#include "src/worker.hpp"

#include <iostream>

BEGIN_VVW_GEN_LIB_NS

Worker::Worker(
    std::function<void(std::unique_lock<std::mutex> &lock)> threadFunction,
    std::function<bool()> hasWorkToDo)
    : threadFunction_(threadFunction),
      hasWorkToDo_(hasWorkToDo),
      workerThread_(&Worker::threadLoop_, this) {}

std::mutex &Worker::getMutex() { return workMutex_; }

Worker::~Worker() { shutdown(); }

void Worker::shutdown() {
  isActive_ = false;
  cv_.notify_one();
  if (workerThread_.joinable()) {
    workerThread_.join();
  }
}

void Worker::setBlock(bool isBlocked) { isBlocked_ = isBlocked; }

void Worker::notifyWorkWasAdded() { cv_.notify_one(); }

void Worker::threadLoop_() {
  while (isActive_) {
    std::unique_lock<std::mutex> lock(workMutex_);
    std::cout << "1\n";
    cv_.wait(lock, [this]() {
      bool workToDo = hasWorkToDo_();
      return (workToDo && !isBlocked_) || !isActive_;
    });
    std::cout << "2\n";
    if (!isActive_) {
      return;
    }

    while (hasWorkToDo_()) {
      if (!isActive_ || isBlocked_) {
        break;
      }
      threadFunction_(lock);
    }
  }
}

END_VVW_GEN_LIB_NS
