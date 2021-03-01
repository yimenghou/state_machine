#ifndef STATE_MACHINE_ENGINE_H
#define STATE_MACHINE_ENGINE_H

#include "state_machine/util.h"
#include "state_machine/state.h"
#include "state_machine/event.h"

#include <type_traits>

namespace sm {

class StateMachineEngine {
public:

  StateMachineEngine() {}
  virtual ~StateMachineEngine() {}

  template <typename T, typename... Args>
  StateMachineEngine& addState(Args &&... data) {
    static_assert(std::is_base_of<StateBase, T>::value, 
      "Accepts only classed derived from StateBase");
    static_assert(!std::is_abstract<T>::value, "Some methods are pure virtual. ");
    state_keeper_.addResource<T>(std::forward<Args>(data)...);
    return *this;
  }

  std::shared_ptr<StateBase> getState(const std::string& name) {
    auto resource = state_keeper_.getResource(name);
    if (resource == nullptr) {
      throw RuntimeError("Cannot find resource with specified name: " + name);
    }
    return state_keeper_.getResource(name);
  }

  inline void setInitialState(const std::string& state) {initial_state_ = state;}

  void start() {
    active_state_ = initial_state_;
    while(1) {
      state_footprint_.push_back(active_state_);
      auto current_state = state_keeper_.getResource(active_state_);
      current_state->tick();
      current_state->setPrev(state_keeper_.getResource(active_state_));
      active_state_ = current_state->getTransitStateID();
    }
  }

  std::stringstream generateFootprint(const std::list<std::string>& list) {
    std::stringstream oss;
    for(std::list<std::string>::const_iterator it = list.begin(); it != list.end(); ++it) {
      if (it == list.begin()) {
        oss << "[" << *it << "]";
      }
      oss << " -> [" << *it << "]";
    }
    return oss;
  }

private:
  std::string initial_state_, active_state_;
  ResourceKeeper<std::string, StateBase> state_keeper_;
  std::list<std::string> state_footprint_;
};

} // namespace sm

#endif