#include "src/worker.hpp"

BEGIN_VVW_GEN_LIB_NS

Worker::Worker(std::function<void()> threadFunction,
               std::function<bool()> hasWorkToDo, std::mutex &workMutex)
    : threadFunction_(threadFunction),
      hasWorkToDo_(hasWorkToDo),
      workMutex_(workMutex),
      workerThread_(&Worker::threadLoop_, this) {}

Worker::~Worker() {
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
    cv_.wait(lock, [this]() {
      bool workToDo = hasWorkToDo_();
      return (workToDo && !isBlocked_) || !isActive_;
    });
    if (!isActive_) {
      return;
    }

    while (hasWorkToDo_()) {
      if (!isActive_ || isBlocked_) {
        break;
      }
      lock.unlock();
      threadFunction_();
      lock.lock();
    }
  }
}

END_VVW_GEN_LIB_NS
