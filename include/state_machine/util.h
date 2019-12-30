#ifndef STATE_MACHINE_UTIL_H
#define STATE_MACHINE_UTIL_H

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

#include "state_machine/exception.h"
namespace sm {

template <typename ID, typename Resource>
class ResourceKeeper {
 public:
  typedef std::unordered_map<ID, std::shared_ptr<Resource>> ResourceMap;

  typedef std::list<ID> ResourceKeyList;

  ResourceKeeper() = default;

  virtual ~ResourceKeeper() {
    for(auto &c : map_) {
      c.second.reset();
    }
  }

  ResourceMap getResourceMap() const { return map_; }

  ResourceKeyList getResourceKeyList() const {
    ResourceKeyList list;
    for(const auto &c : map_) {
      list.push_back(c.first);
    }
    return list;
  }

  int getResourceSize() const { return map_.size(); }

  bool hasResource(const ID& name) {
    if(map_.find(name) != map_.end()) {
      return true;
    }
    return false;
  }

  std::shared_ptr<Resource> getResource(const ID &name) {
    if (!hasResource(name)) {
      return nullptr;
    }
    return map_[name];
  }

  template <typename T, typename... Args>
  ResourceKeeper& addResource(const ID &name, Args &&... data) {
    if(hasResource(name)) {
      throw RuntimeError("try to add a resource that already exists");
    }
    map_.emplace(name, new T(name, std::forward<Args>(data)...));
    return *this;
  }

  bool removeResource(const ID &name) {
    if (hasResource(name)) {
      map_[name]->reset(nullptr);
      map_.erase(name);
      return true;
    }
    return false;
  }

private:
  ResourceMap map_;
};

inline void sleepFor(double time) {
  auto mirco_time = static_cast<int64_t>(time * 1e6);
  std::this_thread::sleep_for(std::chrono::microseconds(mirco_time));
}

#define spinLog(name) std::cout << #name << " spinning" << std::endl;
#define enterLog(name) std::cout << #name << " enter" << std::endl;
#define leaveLog(name) std::cout << #name << " leave" << std::endl;

} // namespace sm

#endif