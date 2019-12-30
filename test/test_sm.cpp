#include "state_machine/sm.h"
#include "state_machine/state/dummy_states.h"

int main(int argc, char **argv) {
  using namespace sm;
  StateMachineEngine sme;

  sme.addResource<StateA>("state_a");
  sme.addResource<StateB>("state_b");
  sme.addResource<StateC>("state_c");

  // sme.getResource("state_a")->listEvents(true);
  // sme.getResource("state_b")->listEvents(true);
  // sme.getResource("state_c")->listEvents(true);

  sme.setInitialState("state_a");
  sme.start();

  return 0;
}