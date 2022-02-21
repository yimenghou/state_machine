#pragma once

#include <cstdint>
#include <any>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <behaviortree_cpp_v3/blackboard.h>

namespace sm {

using Any = BT::Any;

class Blackboard {
 public:
  typedef std::shared_ptr<Blackboard> Ptr;

 protected:
  // This is intentionally protected. Use Blackboard::create instead
  Blackboard() {}

 public:
  /** Use this static method to create an instance of the BlackBoard
   *   to share among all your NodeTrees.
   */
  static Blackboard::Ptr create() { return std::shared_ptr<Blackboard>(); }

  virtual ~Blackboard() = default;

  /**
   * @brief The method getAny allow the user to access directly the type
   * erased value.
   *
   * @return the pointer or nullptr if it fails.
   */
  const Any* getAny(const std::string& key) const {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = storage_.find(key);
    return (it == storage_.end()) ? nullptr : &(it->second);
  }

  Any* getAny(const std::string& key) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = storage_.find(key);
    return (it == storage_.end()) ? nullptr : &(it->second);
  }

  /** Return true if the entry with the given key was found.
   *  Note that this method may throw an exception if the cast to T failed.
   */
  template <typename T>
  bool get(const std::string& key, T& value) const {
    const Any* val = getAny(key);
    if (val) {
      value = std::any_cast<T>(*val);
    }
    return (bool)val;
  }

  /**
   * Version of get() that throws if it fails.
   */
  template <typename T>
  T get(const std::string& key) const {
    const Any* val = getAny(key);
    if (val) {
      return std::any_cast<T>(*val);
    } else {
      // throw RuntimeError("Blackboard::get() error. Missing key [", key, "]");
    }
  }

  /// Update the entry with the given key
  template <typename T>
  void set(const std::string& key, const T& value) {
      std::cout << "!!!" << std::endl;
    std::unique_lock<std::mutex> lock(mutex_);
      std::cout << "!!!" << std::endl;
    const auto& it = storage_.find(key);
      std::cout << "!!" << std::endl;

    if (it != storage_.end())  // already there. check the type
    {
      std::cout << "2" << std::endl;
      auto& previous_any = it->second;
      Any temp(value);
      previous_any = std::move(temp);
    } else {  // create for the first time without any info
      std::cout << "1" << std::endl;
      storage_.emplace(key, value);
    }
  }

  std::vector<std::string_view> getKeys() const {
    if (storage_.empty()) {
      return {};
    }
    std::vector<std::string_view> out;
    out.reserve(storage_.size());
    for (const auto& entry_it : storage_) {
      out.push_back(entry_it.first);
    }
    return out;
  }

  void clear() {
    std::unique_lock<std::mutex> lock(mutex_);
    storage_.clear();
  }

 private:
  mutable std::mutex mutex_;
  std::unordered_map<std::string, Any> storage_;
};

}  // namespace sm