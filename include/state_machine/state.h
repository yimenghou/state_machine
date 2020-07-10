#ifndef STATE_MACHINE_STATE_H
#define STATE_MACHINE_STATE_H

#include "state_machine/return_code.h"
#include "state_machine/event.h"

namespace sm {

class EventPack {
public:
  EventPack(const std::string& event_name_,
            const std::string& transit_to_state,
            const EventF& func,
            Priority priority):
            event_name_(event_name_),
            transit_to_state_(transit_to_state),
            func_(func),
            priority_(priority)
            {}

  std::string event_name() const {return event_name_;}
  std::string transit_to_state() const {return transit_to_state_;}
  Priority priority() const {return priority_;}
  EventF func() {return func_;}

private:
  std::string event_name_;
  std::string transit_to_state_;
  EventF func_;
  Priority priority_;
};

class StateBase {
 public:
  StateBase(const std::string& id) : is_enter_(false), is_terminate_(false), this_state_(id), transit_state_("")  {}
  virtual ~StateBase() {}

  inline std::string getTransitStateID() const { return transit_state_; }

  void setPrev(const std::shared_ptr<StateBase>& another) {
    prev_ = another;
  }

  void tick() {

    sortByPriority(events_);
    listEvents(true);

    if(!is_enter_) {
      onEnter();
      is_enter_ = true;
    }

    while(!is_terminate_) {
      if(ReturnCode::Active == checkCondition()) {
        sleepFor(0.1);
        break;
      }
      spin();
      sleepFor(0.1);
    }

    onLeave();
  }

  std::stringstream listEvents(bool print = true) {
    std::stringstream ss;
    ss << "State [" << this_state_ <<"] has " << events_.size() << " event(s) registered: \n";
    for(const auto& e: events_) {
      ss << "[State: " << this_state_ << "]"; 
      ss << "---[Event: " << e.event_name() << ", priority=" << (e.priority());
      ss << "]---> [State: " << e.transit_to_state() << "]\n";
    }
    if (print) {std::cout << ss.str() << std::endl;}
    return ss;
  }

  template <int priority, typename F = EventF>
  constexpr void registerEvent(const std::string& name, const std::string& transit_to, F&& f) {
    static_assert((priority >= 0) && (priority <= 100), "Priority should be int type with value in range [0, 100]");
    events_.emplace_back(name, transit_to, std::forward<F>(f), priority);
  }

 protected:
  void reset() {
    is_enter_ = false;
    is_terminate_ = false;
  }

  ReturnCode checkCondition() {
    for(auto& e : events_) {
      if(e.func()()) {
        transit_state_ = e.transit_to_state();
        std::cout << "Bring to [State: " << e.transit_to_state() << "] by [Event: " << e.event_name() << "]" << std::endl; 
        return ReturnCode::Active;
      }
    }
    return ReturnCode::Inactive;
  }

  void sortByPriority(std::vector<EventPack>& events) {
    std::sort(events.begin(), events.end(),
    [] (const EventPack& l, const EventPack& r) {
      if ((l.priority()) != (r.priority())) {
        return (l.priority()) > (r.priority());
      }
      return l.transit_to_state() > r.transit_to_state();
    });
  }

  void onEnter() {
    reset();
    onEnterImpl(); 
  }
  
  void onLeave() {
    onLeaveImpl();
  }

  void spin() { spinImpl(); }

  virtual void spinImpl() = 0;
  virtual void onEnterImpl() = 0;
  virtual void onLeaveImpl() = 0;

  bool is_enter_, is_terminate_;
  std::string this_state_, transit_state_;
  std::vector<EventPack> events_;
  std::weak_ptr<StateBase> prev_;
};

} // namespace sm

#endif