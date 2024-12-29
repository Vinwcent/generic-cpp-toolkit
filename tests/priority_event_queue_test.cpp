#include "src/priority_event_queue.hpp"

#include <gtest/gtest.h>

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
