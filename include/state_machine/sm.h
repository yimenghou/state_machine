#pragma once

#include "state_machine/basic_struct.h"
#include "state_machine/util.h"
#include "state_machine/exception.h"
#include "state_machine/blackboard.h"
#include "state_machine/event.h"
#include "state_machine/state.h"

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
    state_keeper_.getResource(state_id)->setBlackBoard(blackboard_);
    state_keeper_.getResource(state_id)->setTickInterval(tick_interval_);
    return *this;
  }

  int StateNumber() const { return state_keeper_.getResourceSize(); }

  BlackboardType::Ptr getBlackboard() { return blackboard_; }

  std::shared_ptr<StateBase> getState(const std::string& name);

  inline std::string getCurrentStateID() const { return active_state_id_ ; }

  inline void setInitialStateID(const std::string& state_id) { active_state_id_ = state_id; }

  inline void setGlobalTickInterval(const DurationType& interval) { tick_interval_ = interval; }

  void spin();

  void spinUntilStateChange();

  std::string generateFootprint(const std::list<std::string>& list);

 private:
  std::string active_state_id_;
  DurationType tick_interval_;
  ResourceKeeper<std::string, StateBase> state_keeper_;
  std::list<std::string> state_footprint_;
  BlackboardType::Ptr blackboard_;
};

}  // namespace sm