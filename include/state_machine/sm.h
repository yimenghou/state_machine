#ifndef STATE_MACHINE_ENGINE_H
#define STATE_MACHINE_ENGINE_H

#include "state_machine/util.h"
#include "state_machine/state.h"
#include "state_machine/event.h"
#include "state_machine/blackboard.h"

#include <type_traits>

namespace sm {

class StateMachineEngine {
public:
  typedef std::shared_ptr<StateMachineEngine> SharedPtr;
  StateMachineEngine();
  virtual ~StateMachineEngine();

  template <typename T>
  StateMachineEngine& addState(const std::string& state_id) {
    static_assert(std::is_base_of<StateBase, T>::value, 
      "Accepts only classed derived from StateBase");
    static_assert(!std::is_abstract<T>::value, "Some methods are pure virtual. ");
    state_keeper_.addResource<T>(state_id);
    std::cout << "2" << std::endl;
    state_keeper_.getResource(state_id)->setBlackBoard(blackboard_);
    std::cout << "3" << std::endl;
    state_keeper_.getResource(state_id)->setTickRate(tick_rate_);
    return *this;
  }

  std::shared_ptr<StateBase> getState(const std::string& name);

  inline void setInitialState(const std::string& state) {initial_state_ = state;}

  inline void setGlobalTickRate(const float rate) { tick_rate_ = rate; }

  void spin();

  std::string generateFootprint(const std::list<std::string>& list);

private:
  std::string initial_state_, active_state_;
  float tick_rate_ = 10;
  ResourceKeeper<std::string, StateBase> state_keeper_;
  std::list<std::string> state_footprint_;
  Blackboard::Ptr blackboard_;
};

} // namespace sm

#endif