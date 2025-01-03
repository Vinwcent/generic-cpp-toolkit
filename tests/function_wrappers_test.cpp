#include "src/function_wrappers.hpp"

#include <gtest/gtest.h>

TEST(FunctionWrapperTest, testUsage) {
  int value = 3;

  std::function<void(int)> valueAdd(
      [&value](int increment) { value += increment; });
  vvw_gen_lib::FunctionWrapper<int> wrapper(valueAdd);

  int incr = 5;
  std::vector<void*> args{&incr};
  wrapper(args);
  EXPECT_EQ(value, 8);
}

TEST(FunctionWrapperTest, testUsageThroughEraser) {
  int value = 3;

  std::function<void(int)> valueAdd(
      [&value](int increment) { value += increment; });
  vvw_gen_lib::FunctionWrapper<int> wrapper(valueAdd);

  std::vector<std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper.push_back(
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueAdd));

  int incr = 4;
  std::vector<void*> args{&incr};
  (*erasedTypeWrapper[0])(args);
  EXPECT_EQ(value, 7);
}

TEST(FunctionWrapperTest, testVectorUsageWithEraser) {
  int value = 3;

  std::function<void(int)> valueAdd(
      [&value](int increment) { value += increment; });
  std::function<void(int)> valueSub(
      [&value](int decrement) { value -= decrement; });

  vvw_gen_lib::FunctionWrapper<int> wrapper(valueAdd);
  std::vector<std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrappers;
  erasedTypeWrappers.push_back(
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueAdd));
  erasedTypeWrappers.push_back(
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueSub));

  int incr = 5;
  int decr = 3;
  std::vector<void*> argsIncr{&incr};
  std::vector<void*> argsDecr{&decr};
  (*erasedTypeWrappers[0])(argsIncr);
  EXPECT_EQ(value, 8);
  (*erasedTypeWrappers[1])(argsDecr);
  EXPECT_EQ(value, 5);
}
