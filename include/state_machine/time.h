#ifndef STATE_MACHINE_TIME_H
#define STATE_MACHINE_TIME_H

#include <chrono>
#include <thread>

namespace sm {

inline void sleepFor(double time) {
  auto mirco_time = static_cast<int64_t>(time * 1e6);
  std::this_thread::sleep_for(std::chrono::microseconds(mirco_time));
}

inline double timeNow() {
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  auto epoch = now_ms.time_since_epoch();
  auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
  return value.count() * 0.001;
}

} // namespace sm

#endif