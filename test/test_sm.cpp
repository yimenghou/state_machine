#include "state_machine/sm.h"
#include "state_machine/state/dummy_states.h"

int main(int argc, char **argv) {
  using namespace sm;
  StateMachineEngine sme;

  sme.addState<StateA>("state_a");
  sme.addState<StateB>("state_b");
  sme.addState<StateC>("state_c");

  // sme.getState("state_a")->listEvents(true);
  // sme.getState("state_b")->listEvents(true);
  // sme.getState("state_c")->listEvents(true);

  sme.setInitialState("state_a");
  sme.start();

  return 0;
}