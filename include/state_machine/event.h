#pragma once

#include "state_machine/basic_struct.h"
#include "state_machine/util.h"
#include "state_machine/exception.h"
#include "state_machine/blackboard.h"

namespace sm {

using BlackboardType = BT::Blackboard;

typedef std::function<bool()> EventFunction;
typedef uint16_t Priority;

class EventBase {
 public:
  EventBase();
  virtual ~EventBase() = default;
  inline void setBlackBoard(const BlackboardType::Ptr& blackboard) { blackboard_ = blackboard; }
  virtual bool update();

 protected:
  virtual void onStart();
  virtual void onLeave();
  std::string uuid_;
  BlackboardType::Ptr blackboard_;
};

}  // namespace sm