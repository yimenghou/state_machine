#ifndef STATE_MACHINE_STATE_H
#define STATE_MACHINE_STATE_H

#include "state_machine/event.h"

namespace sm {

template <typename StateID>
class StateBase {
 public:
  StateBase() : is_enter_(false), is_terminate_(false) {}
  virtual ~StateBase() {}

  inline StateID getTransitStateID() const { return transit_state_; }

  inline void sleepFor(double time) {
    auto mirco_time = static_cast<int64_t>(time * 1e6);
    std::this_thread::sleep_for(std::chrono::microseconds(mirco_time));
  }

  void tick() {
    if(!is_enter_) {
      onEnter();
      is_enter_ = true;
    }

    while(!is_terminate_) {
      if(checkCondition()) {
        sleepFor(0.1);
        break;
      }
      spin();
      sleepFor(0.1);
    }

    onLeave();
  }

  template <typename F = EventF>
  void registerEvent(F&& f, const StateID& next_event) {
    event_map_.emplace(next_event, std::forward<F>(f));
  }

 protected:
  void reset() {
    is_enter_ = false;
    is_terminate_ = false;
  }

  bool checkCondition() {
    for(const auto& e : event_map_) {
      if(e.second()) {
        transit_state_ = e.first;
        return true;
      }
    }
    return false;
  }

  void onEnter() { onEnterImpl(); }
  void onLeave() {
    onLeaveImpl();
    reset();
  }
  void spin() { spinImpl(); }

  virtual void spinImpl() = 0;
  virtual void onEnterImpl() = 0;
  virtual void onLeaveImpl() = 0;

  bool is_enter_, is_terminate_;
  StateID transit_state_;
  std::unordered_map<StateID, EventF> event_map_;
};

class StateA : public StateBase<std::string> {
 public:
  StateA() : event_(5) {
    // register event by member function
    std::function<bool()> ff = std::bind(&CountEvent::update, &event_);
    this->registerEvent(ff, "state_b");
  }
  virtual void spinImpl() override {}
  virtual void onEnterImpl() override {
    event_.start();
    std::cout << "StateA enter" << std::endl;
  }
  virtual void onLeaveImpl() override {
    event_.reset();
    std::cout << "StateA leave" << std::endl;
  }

 private:
  CountEvent event_;
};

class StateB : public StateBase<std::string> {
 public:
  StateB() {
    // register event by lambda function
    auto event = []() {
      std::cout << "event b -> state c" << std::endl;
      return true;
    };

    this->registerEvent(event, "state_c");
  }

  virtual void spinImpl() override {}
  virtual void onEnterImpl() override { std::cout << "StateB enter" << std::endl; }
  virtual void onLeaveImpl() override { std::cout << "StateB leave" << std::endl; }
};

class StateC : public StateBase<std::string> {
 public:
  StateC() : event_(2.0) {
    std::function<bool()> ff = std::bind(&TimeoutEvent::update, &event_);
    this->registerEvent(ff, "state_a");
  }

  virtual void spinImpl() override {}
  virtual void onEnterImpl() override {
    event_.start();
    std::cout << "StateC enter" << std::endl;
  }
  virtual void onLeaveImpl() override {
    event_.reset();
    std::cout << "StateC leave" << std::endl;
  }

 private:
  TimeoutEvent event_;
};

} // namespace sm

#endif