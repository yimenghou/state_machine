#ifndef STATE_MACHINE_EVENT_H
#define STATE_MACHINE_EVENT_H

#include "state_machine/util.h"

namespace sm {

typedef std::function<bool()> EventF;

class Event {
 public:
  Event() {}
  virtual ~Event() {}
protected:
  virtual void reset() = 0;
};

class CountEvent: public Event {
public:
  CountEvent(int max_count):count_(0), max_count_(max_count) {}
  virtual ~CountEvent() = default;

  bool update() {
    count_++;
    if (count_ > max_count_) {
      return true;
    }
    return false;
  }

  virtual void reset() override {
    count_ = 0;
  }

private:
  int count_, max_count_;
};



}

#endif