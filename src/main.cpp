
#include "state_machine/sm.h"

int main(int argc, char **argv) {

  using namespace sm;

  StateMachineEngine sme;

  sme.addResource<StateA>("state_a");
  sme.addResource<StateB>("state_b");
  sme.addResource<StateC>("state_c");
  // sme.addResource<StateA>("state_a");

  sme.setInitialState("state_a");

  sme.start();
  return 0;
}