#ifndef STATE_MACHINE_EVENT_H
#define STATE_MACHINE_EVENT_H

#include "state_machine/util.h"
#include "state_machine/blackboard.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace sm {

typedef std::function<bool()> EventFunction;
typedef uint16_t Priority;

class EventBase {
 public:
  EventBase() {
    boost::uuids::random_generator uuid_gen_;
    uuid_ = boost::uuids::to_string(uuid_gen_());
  }
  virtual ~EventBase() {}
  inline void setBlackBoard(const Blackboard::Ptr& blackboard) { blackboard_ = blackboard; }
  virtual bool update() { return true; }
 protected:
  virtual void onStart() {}
  virtual void onLeave() {}
  std::string uuid_;
  Blackboard::Ptr blackboard_;
};

} // namespace sm

#endif