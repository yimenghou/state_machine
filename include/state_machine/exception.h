#pragma once

#include <stdexcept>
#include <string>

namespace sm {
  
class StateMachineException : public std::runtime_error {
 public:
  StateMachineException(const std::string& message) : std::runtime_error(message) { ; };
};

// This errors are usually related to problems that "probably" require code refactoring
// to be fixed.
class LogicError : public StateMachineException {
 public:
  LogicError(const std::string& message) : StateMachineException(message) {}
};

// This errors are usually related to problems that are relted to data or conditions
// that happen only at run-time
class RuntimeError : public StateMachineException {
 public:
  RuntimeError(const std::string& message) : StateMachineException(message) {}
};

} // namespace sm