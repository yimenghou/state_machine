#ifndef STATE_MACHINE_DUMMY_EVENTS_H
#define STATE_MACHINE_DUMMY_EVENTS_H

#include "state_machine/event.h"

namespace sm {

class CountoutEvent : public EventBase {
 public:
  CountoutEvent(int max_count) : EventBase(), count_(0), max_count_(max_count) {}
  virtual ~CountoutEvent() = default;

  void onStart() {}

  bool update() {
    count_++;
    std::cout << "count_: " << count_  << " of " << max_count_ << std::endl;
    if(count_ >= max_count_) {
      return true;
    }
    return false;
  }

  void onLeave() { count_ = 0; }

 private:
  int count_, max_count_;
};

class TimeoutEvent : public EventBase {
 public:
  using Clock = std::chrono::steady_clock;
  using Duration = std::chrono::duration<double>;
  using TimePoint = std::chrono::time_point<Clock>;

  TimeoutEvent(double timeout) : EventBase(), timeout_(timeout) {}
  virtual ~TimeoutEvent() = default;

  void onStart() { tic_ = Clock::now(); }

  bool update() {
    Duration duration = Clock::now() - tic_;
    std::cout << "duration: " << duration.count()  << " of " << timeout_ << std::endl;
    if(duration.count() > timeout_) {
      return true;
    }
    return false;
  }

  void onLeave() { tic_ = Clock::now(); }

 private:
  TimePoint tic_;
  double timeout_;
};

} // namespace sm

#endif