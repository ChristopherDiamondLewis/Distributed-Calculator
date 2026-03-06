#include <shared_calculator_leader.h>

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <optional>
#include <thread>

namespace Calculator {

Leader::Leader()
    : d_currValue(0),
      d_rng(std::random_device{}()),
      d_opDistribution(0, d_supportedOperations.size() - 1),
      d_argDistribution(1, 100),
      d_eventGenerationMsDistribution(10, 1000) {}

void Leader::Run() {
  while (true) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(d_eventGenerationMsDistribution(d_rng)));
    SubmitEvent(CreateRandomEvent());
    std::cout << "Current value: " << d_currValue << std::endl;
  }
}

std::optional<Events> Leader::WaitForUpdatesFromIndex(
    const size_t fromIndex, const std::chrono::milliseconds timeout) {
  std::unique_lock<std::mutex> lock(d_mutex);

  const bool updates_available = d_updates_available_cv.wait_for(
      lock, timeout, [&] { return d_events.size() > fromIndex; });

  if (not updates_available) return std::nullopt;

  return GetUpdatesFromWithLock(fromIndex);
}

Events Leader::GetUpdatesFromWithLock(const size_t fromIndex) const {
  // return events from log starting at fromIndex
  Events updates;
  for (size_t i = fromIndex; i < d_events.size(); i++) {
    updates.push_back(d_events[i]);
  }
  return updates;
}

void Leader::SubmitEvent(Event event) {
  // add event to log and update state
  {
    std::lock_guard<std::mutex> lock(d_mutex);
    event.d_eventIndex = d_events.size();
    d_events.push_back(event);
    d_currValue = Calculator::Utility::ApplyCalculation(event, d_currValue);
  }
  std::cout << "submitting event : " << event << std::endl;

  d_updates_available_cv.notify_all();
}

std::pair<int64_t, size_t> Leader::GetCurrentValueAndIndex() const {
  std::lock_guard<std::mutex> lock(d_mutex);
  return {d_currValue, d_events.size()};
}

Event Leader::CreateRandomEvent() {
  Event event;
  event.d_operation = d_supportedOperations[d_opDistribution(d_rng)];
  event.d_argument = d_argDistribution(d_rng);
  return event;
}

}  // namespace Calculator