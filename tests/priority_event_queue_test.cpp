#include "src/priority_event_queue.hpp"

#include <gtest/gtest.h>

#include "src/priority_event_queue_builder.hpp"

enum class DummyEvent { EVENT_1 };

TEST(PriorityEventQueueTest, construction) {
  int value = 1;
  std::function<void(int)> valueAdd(
      [&value](int increment) { value += increment; });
  vvw_gen_lib::FunctionWrapper<int> wrapper(valueAdd);

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueAdd);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  EXPECT_NO_THROW({
    auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  });
}

TEST(PriorityEventQueueTest, addEventTest) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int, int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.startProcessingEvents();

  queue.addEvent(DummyEvent::EVENT_1, 4, 1);
  queue.addEvent(DummyEvent::EVENT_1, 6, 1);
  queue.addEvent(DummyEvent::EVENT_1, 8, 1);

  EXPECT_EQ(queue.getNPendingEvents(DummyEvent::EVENT_1), 3);
}

TEST(PriorityEventQueueTest, processEvent) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int, int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.startProcessingEvents();
  queue.addEvent(DummyEvent::EVENT_1, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 5);
  queue.addEvent(DummyEvent::EVENT_1, 3, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 18);
}

TEST(PriorityEventQueueTest, processEventWhileAdding) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int, int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.startProcessingEvents();
  queue.addEvent(DummyEvent::EVENT_1, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 5);
  for (int i = 0; i < 100; ++i) {
    queue.addEvent(DummyEvent::EVENT_1, 1, 1);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 105);
}

TEST(PriorityEventQueueBuilderTest, builder) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  vvw_gen_lib::PriorityEventQueueBuilder<DummyEvent> builder{};
  builder.registerEvent(DummyEvent::EVENT_1, 1, valueAffine);

  auto queue = builder.build();
  queue->startProcessingEvents();
  queue->addEvent(DummyEvent::EVENT_1, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 5);
}
