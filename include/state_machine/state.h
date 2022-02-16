#ifndef STATE_MACHINE_STATE_H
#define STATE_MACHINE_STATE_H

#include <optional>

#include "state_machine/basic_struct.h"
#include "state_machine/blackboard.h"
#include "state_machine/event.h"
#include "state_machine/util.h"

namespace sm {

class EventPack {
 public:
  EventPack(const std::string& name, const std::string& to_state, Priority priority,
            const EventFunction& func)
      : name_(name), to_state_(to_state), priority_(priority), func_(func) {}

  EventPack(const std::string& name, const std::string& to_state, Priority priority,
            EventBase* event)
      : name_(name), to_state_(to_state), priority_(priority), event_(event) {}

  std::string name() const { return name_; }
  std::string to_state() const { return to_state_; }
  Priority priority() const { return priority_; }
  std::optional<EventFunction> func() { return func_; }
  EventBase* event() const { return event_; }

 private:
  std::string name_;
  std::string to_state_;
  Priority priority_;
  std::optional<EventFunction> func_;
  EventBase* event_;
};

class StateBase {
 public:
  typedef std::shared_ptr<StateBase> SharedPtr;
  StateBase(const std::string& id);
  virtual ~StateBase();

  inline void setBlackBoard(const Blackboard::Ptr& blackboard) { blackboard_ = blackboard; }

  inline std::string getTransitStateID() const { return transit_state_; }

  inline void setPrev(const std::shared_ptr<StateBase>& another) { prev_ = another; }

  inline void setTickRate(const float rate) { tick_rate_ = rate; }

  void tick();

  std::string listEvents();

  template <int priority>
  constexpr void registerEvent(const std::string& name, const std::string& transit_to,
                               EventFunction func) {
    static_assert((priority >= 0) && (priority <= 100),
                  "Priority should be int type with value in range [0, 100]");
    events_.emplace_back(name, transit_to, priority, func);
  }

  template <int priority>
  constexpr void registerEvent(const std::string& name, const std::string& transit_to,
                               EventBase* event) {
    static_assert((priority >= 0) && (priority <= 100),
                  "Priority should be int type with value in range [0, 100]");
    events_.emplace_back(name, transit_to, priority, event);
    events_.back().event()->setBlackBoard(blackboard_);
  }

 protected:
  void reset();

  bool checkCondition();

  void sortByPriority(std::vector<EventPack>& events);
  void onEnter();
  void onLeave();
  void update();

  virtual void UpdateImpl() = 0;
  virtual void onEnterImpl() = 0;
  virtual void onLeaveImpl() = 0;

  bool is_enter_, is_terminate_;
  float tick_rate_;
  std::string this_state_, transit_state_;
  std::vector<EventPack> events_;
  std::weak_ptr<StateBase> prev_;
  Blackboard::Ptr blackboard_;
};

}  // namespace sm

#endif