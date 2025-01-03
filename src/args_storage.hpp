#ifndef VVW_ARGS_STORAGE_HPP
#define VVW_ARGS_STORAGE_HPP

#include <vector>

#include "src/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

struct ArgsStorageTypeEraser {
  virtual ~ArgsStorageTypeEraser() = default;
  virtual std::vector<void*> getArgPtrs() = 0;
};

template <typename... Args>
struct ArgsStorage : ArgsStorageTypeEraser {
  std::tuple<Args...> args_;

  explicit ArgsStorage(Args... values) : args_(values...) {}

  std::vector<void*> getArgPtrs() override {
    return getPtrsImpl_(std::index_sequence_for<Args...>{});
  }

  std::tuple<Args...> getArgsTuple() const { return args_; }

 private:
  template <size_t... Is>
  std::vector<void*> getPtrsImpl_(std::index_sequence<Is...>) {
    return std::vector<void*>{&std::get<Is>(args_)...};
  }
};

END_VVW_GEN_LIB_NS

#endif
