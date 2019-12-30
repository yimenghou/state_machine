#ifndef STATE_MACHINE_DUMMY_STATES_H
#define STATE_MACHINE_DUMMY_STATES_H

#include "state_machine/event.h"
#include "state_machine/event/dummy_events.h"
#include "state_machine/state.h"

namespace sm {

class StateA : public StateBase {
 public:
  StateA(const std::string& id) : StateBase(id), event0_(5), event1_(20), event2_(30) {
    // register event by member function
    std::function<bool()> ff0 = std::bind(&CountoutEvent::update, &event0_);
    std::function<bool()> ff1 = std::bind(&CountoutEvent::update, &event1_);
    std::function<bool()> ff2 = std::bind(&CountoutEvent::update, &event2_);
    this->registerEvent<0>("dummy_event_0", "state_b", ff0);
    this->registerEvent<92>("dummy_event_1", "state_c", ff1);
    this->registerEvent<30>("dummy_event_2", "state_c", ff2);
  }
  virtual void spinImpl() override {spinLog(StateA);}
  virtual void onEnterImpl() override {
    event0_.start();
    event1_.start();
    event2_.start();
    enterLog(StateC);
  }
  virtual void onLeaveImpl() override {
    event0_.reset();
    event1_.reset();
    event2_.reset();
    leaveLog(StateC);
  }

 private:
  CountoutEvent event0_;
  CountoutEvent event1_;
  CountoutEvent event2_;
};

class StateB : public StateBase {
 public:
  StateB(const std::string& id) : StateBase(id) {
    // register event by lambda function
    auto event = []() {return true;};

    this->registerEvent<12>("dummy_event_0", "state_c", event);
  }

  virtual void spinImpl() override {spinLog(StateB);}
  virtual void onEnterImpl() override {enterLog(StateB);}
  virtual void onLeaveImpl() override {leaveLog(StateB);}
};

class StateC : public StateBase {
 public:
  StateC(const std::string& id) : StateBase(id), event_(2.0) {
    std::function<bool()> ff = std::bind(&TimeoutEvent::update, &event_);
    this->registerEvent<88>("dummy_event_0", "state_a", ff);
  }

  virtual void spinImpl() override {
    spinLog(StateC);
  }
  virtual void onEnterImpl() override {
    event_.start();
    enterLog(StateC);
  }
  virtual void onLeaveImpl() override {
    event_.reset();
    leaveLog(StateC);
  }

 private:
  TimeoutEvent event_;
};

} // namespace sm

#endif