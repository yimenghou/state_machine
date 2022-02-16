#pragma once

#define FMT_HEADER_ONLY

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <unistd.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <type_traits>
#include <functional>
#include <chrono>
#include <thread>
#include <tuple>
#include <shared_mutex>
#include <mutex>

#include "state_machine/exception.h"
namespace sm {

template <typename ID, typename ResourceT>
class ResourceKeeper {
 public:
  typedef std::unordered_map<ID, std::shared_ptr<ResourceT>> ResourceMap;

  ResourceKeeper() = default;

  virtual ~ResourceKeeper() {
    for(auto &c : map_) {
      c.second.reset();
    }
  }

  ResourceMap getResourceMap() const { return map_; }

  int getResourceSize() const { return map_.size(); }

  bool hasResource(const ID& name) {
    if(map_.find(name) != map_.end()) {
      return true;
    }
    return false;
  }

  std::shared_ptr<ResourceT> getResource(const ID &name) {
    std::shared_lock<std::shared_mutex> lock(mtx_);
    if (!hasResource(name)) {
      return nullptr;
    }
    return map_[name];
  }

  template <typename T, typename... Args>
  ResourceKeeper& addResource(const ID &name, Args &&... data) {
    std::unique_lock<std::shared_mutex> lock(mtx_);
    if(hasResource(name)) {
      throw RuntimeError("try to add a resource that already exists");
    }
    map_.emplace(name, new T(name, std::forward<Args>(data)...));
    return *this;
  }

  bool removeResource(const ID &name) {
    std::unique_lock<std::shared_mutex> lock(mtx_);
    if (hasResource(name)) {
      map_[name]->reset(nullptr);
      map_.erase(name);
      return true;
    }
    return false;
  }

private:
  mutable std::shared_mutex mtx_;
  ResourceMap map_;
};

inline void sleepFor(double time) {
  auto mirco_time = static_cast<int64_t>(time * 1e6);
  std::this_thread::sleep_for(std::chrono::microseconds(mirco_time));
}

#define updateLog(name) std::cout << #name << " update" << std::endl;
#define enterLog(name) std::cout << #name << " enter" << std::endl;
#define leaveLog(name) std::cout << #name << " leave" << std::endl;

} // namespace sm