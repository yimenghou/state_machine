#include <state_machine/state.h>

namespace sm {

StateBase::StateBase(const std::string& id)
    : is_enter_(false), is_terminate_(false), this_state_(id), transit_state_("") {}

StateBase::~StateBase() {}

void StateBase::tick() {
  sortByPriority(events_);
  listEvents();

  if (!is_enter_) {
    onEnter();
    is_enter_ = true;
  }

  while (!is_terminate_) {
    if (checkCondition()) {
      break;
    }
    update();
    sleepFor(1 / tick_rate_);
  }

  onLeave();
}

std::string StateBase::listEvents() {
  auto title =
      fmt::format("State {} has {} event(s)\n", this_state_, events_.size());
  std::string events("");
  for (const auto& e : events_) {
    events += fmt::format("[State: {}]---[Event: {}, priority={}]---> [State: {}]\n", this_state_,
                          e.name(), e.priority(), e.to_state());
  }
  std::cout << title << events;
  return title + events;
}

void StateBase::reset() {
  is_enter_ = false;
  is_terminate_ = false;
}

bool StateBase::checkCondition() {
  for (auto& e : events_) {
    if (e.func().has_value()) {
      std::cout << fmt::format("Check event (func) name: {}, to: {}, priority: {}\n", e.name(),
                               e.to_state(), e.priority());
      if (e.func().value()()) {
        transit_state_ = e.to_state();
        std::cout << "Bring to [State: " << e.to_state() << "] by [Event: " << e.name() << "]"
                  << std::endl;
        return true;
      }
    }
    if (e.event()) {
      std::cout << fmt::format("Check event (class) name: {}, to: {}, priority: {}\n", e.name(),
                               e.to_state(), e.priority());
      if (e.event()->update()) {
        transit_state_ = e.to_state();
        std::cout << "Bring to [State: " << e.to_state() << "] by [Event: " << e.name() << "]"
                  << std::endl;
        return true;
      }
    }
  }
  return false;
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
  reset();
  onEnterImpl();
}

void StateBase::onLeave() { onLeaveImpl(); }

void StateBase::update() { UpdateImpl(); }

}  // namespace sm