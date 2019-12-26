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
#include <unordered_map>
#include <vector>

namespace sm {

template <typename ID, typename Resource>
class ResourceKeeper {
 public:
  typedef std::unordered_map<ID, std::shared_ptr<Resource>> ResourceMap;

  typedef std::list<ID> ResourceKeyList;

  ResourceKeeper() {}
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

  std::shared_ptr<Resource> getResource(const ID &name) {
    if(map_.find(name) == map_.end()) {
      return nullptr;
    }
    return map_[name];
  }
  template <typename T, typename... Args>
  void addResource(const ID &name, Args &&... data) {
    map_.emplace(name, new T(std::forward<Args>(data)...));
  }

  bool removeResource(const ID &name) {
    if(map_.find(name) != map_.end()) {
      map_[name]->reset(nullptr);
      map_.erase(name);
      return true;
    }
    return false;
  }

private:
  ResourceMap map_;
};



/**
 * @class Factory
 * @brief Implements a Factory design pattern with Register and Create methods
 *
 * The objects created by this factory all implement the same interface
 * (namely, AbstractProduct). This design pattern is useful in settings where
 * multiple implementations of an interface are available, and one wishes to
 * defer the choice of the implementation in use.
 *
 * @param IdentifierType Type used for identifying the registered classes,
 * typically std::string.
 * @param AbstractProduct The interface implemented by the registered classes
 * @param ProductCreator Function returning a pointer to an instance of
 * the registered class
 * @param MapContainer Internal implementation of the function mapping
 * IdentifierType to ProductCreator, by default std::unordered_map
 */
template <typename IdentifierType, class AbstractProduct,
          class ProductCreator = AbstractProduct *(*)(),
          class MapContainer =
              std::unordered_map<IdentifierType, ProductCreator>>
class Factory {
 public:
  /**
   * @brief Registers the class given by the creator function, linking it to id.
   * Registration must happen prior to calling CreateObject.
   * @param id Identifier of the class being registered
   * @param creator Function returning a pointer to an instance of
   * the registered class
   * @return True iff the key id is still available
   */
  bool Register(const IdentifierType &id, ProductCreator creator) {
    return producers_.insert(std::make_pair(id, creator)).second;
  }

  bool Contains(const IdentifierType &id) {
    return producers_.find(id) != producers_.end();
  }

  /**
   * @brief Unregisters the class with the given identifier
   * @param id The identifier of the class to be unregistered
   */
  bool Unregister(const IdentifierType &id) {
    return producers_.erase(id) == 1;
  }

  void Clear() { producers_.clear(); }

  bool Empty() const { return producers_.empty(); }

  /**
   * @brief Creates and transfers membership of an object of type matching id.
   * Need to register id before CreateObject is called. May return nullptr
   * silently.
   * @param id The identifier of the class we which to instantiate
   * @param args the object construction arguments
   */
  template <typename... Args>
  std::unique_ptr<AbstractProduct> CreateObjectOrNull(const IdentifierType &id,
                                                      Args &&... args) {
    auto id_iter = producers_.find(id);
    if (id_iter != producers_.end()) {
      return std::unique_ptr<AbstractProduct>(
          (id_iter->second)(std::forward<Args>(args)...));
    }
    return nullptr;
  }

  /**
   * @brief Creates and transfers membership of an object of type matching id.
   * Need to register id before CreateObject is called.
   * @param id The identifier of the class we which to instantiate
   * @param args the object construction arguments
   */
  template <typename... Args>
  std::unique_ptr<AbstractProduct> CreateObject(const IdentifierType &id,
                                                Args &&... args) {
    auto result = CreateObjectOrNull(id, std::forward<Args>(args)...);
    return result;
  }

 private:
  MapContainer producers_;
};

} // namespace sm

#endif