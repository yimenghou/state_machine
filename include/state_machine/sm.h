#ifndef STATE_MACHINE_ENGINE_H
#define STATE_MACHINE_ENGINE_H

#include "state_machine/util.h"
#include "state_machine/state.h"
#include "state_machine/event.h"
#include "state_machine/condition.h"

namespace sm {

class StateMachineEngine {
public:

  StateMachineEngine() {
    state_keeper_.addResource<StateA>("state_a");
    state_keeper_.addResource<StateB>("state_b");
    state_keeper_.addResource<StateC>("state_c");
  }

  inline void setInitialState(const std::string& state) {
    initial_state_ = state;
  }

  virtual ~StateMachineEngine() {}

  void run() {
    active_state_ = initial_state_;
    while(1) {
      state_footprint_.push_back(active_state_);
      std::cout << generateFootprint(state_footprint_).str() << std::endl;
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