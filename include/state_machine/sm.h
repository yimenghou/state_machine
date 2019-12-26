#ifndef STATE_MACHINE_ENGINE_H
#define STATE_MACHINE_ENGINE_H

#include <type_traits>

#include "state_machine/util.h"
#include "state_machine/state.h"
#include "state_machine/event.h"

namespace sm {

class StateMachineEngine {
public:

  StateMachineEngine() {}

  template <typename T, typename... Args>
  StateMachineEngine& addResource(Args &&... data) {
    static_assert(std::is_base_of_v<StateBase<std::string>, T>, 
      "Accepts only classed derived from StateBase<std::string>");
    static_assert(!std::is_abstract_v<T>, "Some methods are pure virtual. ");
    state_keeper_.addResource<T>(std::forward<Args>(data)...);
    return *this;
  }

  inline void setInitialState(const std::string& state) {
    initial_state_ = state;
  }

  virtual ~StateMachineEngine() {}

  void start() {
    active_state_ = initial_state_;
    while(1) {
      state_footprint_.push_back(active_state_);
      auto current_state = state_keeper_.getResource(active_state_);
      current_state->tick();
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
  ResourceKeeper<std::string, StateBase<std::string>> state_keeper_;
  std::list<std::string> state_footprint_;
};

} // namespace sm

#endif