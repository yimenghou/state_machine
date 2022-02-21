
#include "state_machine/sm.h"

namespace sm {

StateMachineEngine::StateMachineEngine() { blackboard_ = BlackboardType::create(); }

StateMachineEngine::~StateMachineEngine() {}

std::shared_ptr<StateBase> StateMachineEngine::getState(const std::string& name) {
  auto resource = state_keeper_.getResource(name);
  if (resource == nullptr) {
    throw RuntimeError("Cannot find resource with specified name: " + name);
  }
  return state_keeper_.getResource(name);
}

void StateMachineEngine::spin() {
  std::cout << fmt::format("Start from initial state: {}\n", active_state_id_);
  while (1) {
    spinUntilStateChange();
  }
}

void StateMachineEngine::spinUntilStateChange() {
  state_footprint_.push_back(active_state_id_);
  auto current_state = state_keeper_.getResource(active_state_id_);
  current_state->setLastStateID(active_state_id_);
  current_state->spin();
  current_state->setPrev(state_keeper_.getResource(active_state_id_));
  active_state_id_ = current_state->getNextStateID();
}

std::string generateFootprint(const std::list<std::string>& list) {
  std::string fpt("");
  for (std::list<std::string>::const_iterator it = list.begin(); it != list.end(); ++it) {
    if (it == list.begin()) {
      fpt += fmt::format("[{}]", *it);
    }
    fpt += fmt::format(" -> [{}]", *it);
  }
  return fpt;
}

}  // namespace sm