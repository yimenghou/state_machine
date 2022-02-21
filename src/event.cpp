#include "state_machine/event.h"

namespace sm {

EventBase::EventBase() {
  boost::uuids::random_generator uuid_gen_;
  uuid_ = boost::uuids::to_string(uuid_gen_());
}

bool EventBase::update() { return true; }

void EventBase::onStart() {}

void EventBase::onLeave() {}

}  // namespace sm