#include "state_machine/state.h"

namespace sm {

StateBase::StateBase(const std::string& id)
    : is_enter_(false),
      is_terminate_(false),
      id_(id),
      last_state_id_(""),
      next_state_id_(""),
      trigger_event_id_(""),
      tic_(0.), toc_(0.), duration_(0.) {}

StateBase::~StateBase() {}

void StateBase::spin() {
  if (!is_enter_) {
    onEnter();
    is_enter_ = true;
  }

  sortByPriority(events_);
  listEvents();

  while (!is_terminate_) {
    if (checkCondition()) {
      break;
    }
    update();
    std::this_thread::sleep_for(tick_interval_);
  }

  onLeave();
}

std::string StateBase::listEvents() {
  auto title = fmt::format("State {} has {} event(s)\n", id_, events_.size());
  std::string events("");
  for (const auto& e : events_) {
    events += fmt::format("[State: {}]---[Event: {}, priority={}]---> [State: {}]\n", id_, e.name(),
                          e.priority(), e.to_state());
  }
  std::cout << title << events;
  return title + events;
}

void StateBase::reset() {
  is_enter_ = false;
  is_terminate_ = false;
  id_ = "";
  last_state_id_ = "";
  next_state_id_ = "";
  trigger_event_id_ = "";
  tic_ = 0.;
  toc_ = 0.;
  duration_ = 0;
}

bool StateBase::checkCondition() {
  // check each events (sorted by priority) see if condition met)
  bool trigger(false);
  for (auto& e : events_) {
    if (e.func().has_value()) {
      std::cout << fmt::format("Check event (func) name: {}, to: {}, priority: {}\n", e.name(),
                               e.to_state(), e.priority());
      if (e.func().value()()) {
        next_state_id_ = e.to_state();
        std::cout << "Bring to [State: " << e.to_state() << "] by [Event: " << e.name() << "]"
                  << std::endl;
        trigger = true;
      }
    }
    if (e.event()) {
      std::cout << fmt::format("Check event (class) name: {}, to: {}, priority: {}\n", e.name(),
                               e.to_state(), e.priority());
      if (e.event()->update()) {
        next_state_id_ = e.to_state();
        std::cout << "Bring to [State: " << e.to_state() << "] by [Event: " << e.name() << "]"
                  << std::endl;
        trigger = true;
      }
    }
  }

  return trigger;
}

void StateBase::sortByPriority(std::vector<EventPack>& events) {
  std::sort(events.begin(), events.end(), [](const EventPack& l, const EventPack& r) {
    if ((l.priority()) != (r.priority())) {
      return (l.priority()) > (r.priority());
    }
    return l.to_state() > r.to_state();
  });
}

void StateBase::onEnter() {
  onEnterImpl();
  reset();
  tic_ = timeNow();
}

void StateBase::onLeave() {
  onLeaveImpl();
  toc_ = timeNow();
  duration_ = toc_ - tic_;
}

void StateBase::update() { UpdateImpl(); }

}  // namespace sm