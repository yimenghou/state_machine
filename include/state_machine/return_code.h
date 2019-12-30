#ifndef STATE_MACHINE_RETURN_CODE_H
#define STATE_MACHINE_RETURN_CODE_H

#include <stdexcept>
#include <string>
namespace sm {
  
enum class ReturnCode:uint8_t {
  Active=0,
  Inactive=1
};

} // namespace sm

#endif