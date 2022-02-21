#include "state_machine/event.h"
#include "state_machine/sm.h"
#include "state_machine/state.h"

#include <gtest/gtest.h>

namespace sm {

class DummyData {
 public:
  float a = 3.2;
  double b = -4.4;
  int c = 1;
  bool d = true;
  std::string e = "hello";
};

class CountoutEvent : public EventBase {
 public:
  CountoutEvent(int max_count) : EventBase(), count_(0), max_count_(max_count) {}
  virtual ~CountoutEvent() = default;

  void onStart() {
    assert(blackboard_);
    auto dd = blackboard_->get<std::shared_ptr<DummyData>>("dummy_data");
  }

  bool update() {
    count_++;
    std::cout << "count_: " << count_ << " of " << max_count_ << std::endl;
    if (count_ >= max_count_) {
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
    std::cout << "duration: " << duration.count() << " of " << timeout_ << std::endl;
    if (duration.count() > timeout_) {
      return true;
    }
    return false;
  }

  void onLeave() { tic_ = Clock::now(); }

 private:
  TimePoint tic_;
  double timeout_;
};

class StateA : public StateBase {
 public:
  StateA(const std::string& id) : StateBase(id), event0_(5), event1_(20) {}
  virtual void UpdateImpl() override { updateLog(StateA); }
  virtual void onEnterImpl() override {
    this->registerEvent<50>("dummy_event_a0", "state_b", &event0_);
    this->registerEvent<51>("dummy_event_a1", "state_b", &event1_);
    event0_.onStart();
    event1_.onStart();
    enterLog(StateA);
  }
  virtual void onLeaveImpl() override {
    event0_.onLeave();
    event1_.onLeave();
    leaveLog(StateA);
  }

 private:
  CountoutEvent event0_;
  CountoutEvent event1_;
};

class StateB : public StateBase {
 public:
  StateB(const std::string& id) : StateBase(id) {}

  virtual void UpdateImpl() override { updateLog(StateB); }
  virtual void onEnterImpl() override {
    enterLog(StateB);

    // register event by lambda function
    auto event = []() { return true; };

    this->registerEvent<12>("dummy_event_b0", "state_c", event);
  }
  virtual void onLeaveImpl() override { leaveLog(StateB); }
};

class StateC : public StateBase {
 public:
  StateC(const std::string& id) : StateBase(id), event_(2.0) {}

  virtual void UpdateImpl() override { updateLog(StateC); }
  virtual void onEnterImpl() override {
    std::function<bool()> ff = std::bind(&TimeoutEvent::update, &event_);
    this->registerEvent<88>("dummy_event_c0", "state_a", ff);

    event_.onStart();
    enterLog(StateC);
  }
  virtual void onLeaveImpl() override {
    event_.onLeave();
    leaveLog(StateC);
  }

 private:
  TimeoutEvent event_;
};

class StateMachineTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
  sm::StateMachineEngine sme;
  std::shared_ptr<sm::DummyData> dd;
};

TEST(BlackboardTest, Test1) {
  auto bb = BlackboardType::create();
  auto dd = std::make_shared<sm::DummyData>();
  bb->set<std::shared_ptr<sm::DummyData>>("dd", dd);
  auto bb_get = bb->get<std::shared_ptr<sm::DummyData>>("dd");

  ASSERT_NE(bb_get, nullptr);
  EXPECT_EQ(bb_get->a, dd->a);
  EXPECT_EQ(bb_get->b, dd->b);
  EXPECT_EQ(bb_get->c, dd->c);
  EXPECT_EQ(bb_get->d, dd->d);
  EXPECT_EQ(bb_get->e, dd->e);

  float a = 5.2;
  double b = 0.37;
  int c = 398263;
  bool d = false;
  std::string e = "helloworld";
  bb_get->a = a;
  bb_get->b = b;
  bb_get->c = c;
  bb_get->d = d;
  bb_get->e = e;

  EXPECT_EQ(bb_get->a, a);
  EXPECT_EQ(bb_get->b, b);
  EXPECT_EQ(bb_get->c, c);
  EXPECT_EQ(bb_get->d, d);
  EXPECT_EQ(bb_get->e, e);  
}

TEST_F(StateMachineTest, Test1) {

  sme.setGlobalTickInterval(std::chrono::milliseconds(10));
  sme.getBlackboard()->set<std::shared_ptr<sm::DummyData>>("dummy_data", dd);

  sme.addState<StateA>("state_a");
  EXPECT_EQ(sme.StateNumber(), 1);
  sme.addState<StateB>("state_b");
  EXPECT_EQ(sme.StateNumber(), 2);
  sme.addState<StateC>("state_c");
  EXPECT_EQ(sme.StateNumber(), 3);

  auto state_a =  sme.getState("state_a");
  auto state_b =  sme.getState("state_b");
  auto state_c =  sme.getState("state_c");
  ASSERT_NE(state_a, nullptr);
  ASSERT_NE(state_b, nullptr);
  ASSERT_NE(state_c, nullptr);

  sme.setInitialStateID("state_a");
  sme.spinUntilStateChange();
  EXPECT_EQ(sme.getCurrentStateID(), "state_b"); // state_a -> state_b
  sme.spinUntilStateChange();
  EXPECT_EQ(sme.getCurrentStateID(), "state_c"); // state_b -> state_c
  sme.spinUntilStateChange();
  EXPECT_EQ(sme.getCurrentStateID(), "state_a"); // state_c -> state_a
  std::cout << "Duration a: " << state_a->getDuration() << std::endl;
  std::cout << "Duration b:" << state_b->getDuration() << std::endl;
  std::cout << "Duration c:" << state_c->getDuration() << std::endl;
}

}  // namespace sm