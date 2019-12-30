#ifndef STATE_MACHINE_EVENT_H
#define STATE_MACHINE_EVENT_H

#include "state_machine/util.h"

namespace sm {

typedef std::function<bool()> EventF;
typedef uint16_t Priority;

class Event {
 public:
  Event():priority_(0) {}
  virtual ~Event() {}

  void setPriority(Priority priority) {priority_ = priority;}
 
 protected:
  virtual bool update() = 0;
  virtual void reset() = 0;
  virtual void start() = 0;
 
  Priority priority_;
};

} // namespace sm

#endif