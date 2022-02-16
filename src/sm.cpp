
#include "state_machine/sm.h"

namespace sm {

StateMachineEngine::StateMachineEngine() {
  std::cout << "1" << std::endl;
  blackboard_ = Blackboard::create();
  std::cout << "2" << std::endl;
}

StateMachineEngine::~StateMachineEngine() {}

std::shared_ptr<StateBase> StateMachineEngine::getState(const std::string& name) {
  auto resource = state_keeper_.getResource(name);
  if (resource == nullptr) {
    throw RuntimeError("Cannot find resource with specified name: " + name);
  }
  return state_keeper_.getResource(name);
}

void StateMachineEngine::spin() {
  active_state_ = initial_state_;
  std::cout << fmt::format("Initial state: {}\n", active_state_);
  while(1) {
    state_footprint_.push_back(active_state_);
    auto current_state = state_keeper_.getResource(active_state_);
    current_state->tick();
    current_state->setPrev(state_keeper_.getResource(active_state_));
    active_state_ = current_state->getTransitStateID();
  }
}

std::string generateFootprint(const std::list<std::string>& list) {
  std::string fpt("");
  for(std::list<std::string>::const_iterator it = list.begin(); it != list.end(); ++it) {
    if (it == list.begin()) {
      fpt += fmt::format("[{}]", *it);
    }
    fpt += fmt::format(" -> [{}]", *it);
  }
  return fpt;
}

}