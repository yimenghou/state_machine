
#include "state_machine/sm.h"

int main(int argc, char **argv) {
  sm::StateMachineEngine sme;
  sme.setInitialState("state_a");
  sme.run();
  return 0;
}