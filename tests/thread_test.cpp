#include <gtest/gtest.h>

#include "src/worker.hpp"

TEST(WorkerTest, usage) {
  int value = 1;
  std::function<void()> increaseValue([&value]() { value += 1; });
  std::function<bool()> hasWorkToDo(
      [&value]() { return value > 1 && value < 10; });
  std::mutex mtx;
  vvw_gen_lib::Worker worker(increaseValue, hasWorkToDo, mtx);
  EXPECT_EQ(value, 1);
  value += 1;
  EXPECT_EQ(value, 2);
  worker.notifyWorkWasAdded();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 10);
}

TEST(WorkerTest, block) {
  int value = 1;
  std::function<void()> increaseValue([&value]() { value += 1; });
  std::function<bool()> hasWorkToDo(
      [&value]() { return value > 1 && value < 10; });
  std::mutex mtx;
  vvw_gen_lib::Worker worker(increaseValue, hasWorkToDo, mtx);
  EXPECT_EQ(value, 1);
  value += 1;
  EXPECT_EQ(value, 2);
  worker.setBlock(true);
  worker.notifyWorkWasAdded();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 2);
}
