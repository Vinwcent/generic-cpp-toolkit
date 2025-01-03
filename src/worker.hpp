#ifndef VVW_WORKER_HPP
#define VVW_WORKER_HPP

#include <functional>
#include <thread>

#include "src/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

class Worker {
 public:
  Worker(std::function<void(std::unique_lock<std::mutex> &lock)> threadFunction,
         std::function<bool()> hasWorkToDo);
  ~Worker();

  void setBlock(bool isBlocked);
  void notifyWorkWasAdded();

 private:
  std::thread workerThread_;
  std::atomic<bool> isActive_ = true;
  std::atomic<bool> isBlocked_ = false;
  std::condition_variable cv_;

  std::function<void(std::unique_lock<std::mutex> &lock)> threadFunction_;
  std::function<bool()> hasWorkToDo_;
  std::mutex workMutex_;

  void threadLoop_();
};

END_VVW_GEN_LIB_NS

#endif
