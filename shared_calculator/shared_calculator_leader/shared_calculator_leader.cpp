#include <shared_calculator_leader.h>

#include <chrono>
#include <optional>
#include <thread>

namespace Calculator {

Leader::Leader() : d_currValue(0), d_lastWrittenIndex(0){};

bool Leader::Run() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const auto event = CreateRandomEvent();
    SubmitEvent(event);
    std::cout << "Current value: " << d_currValue << std::endl;
  }

  return true;
}

std::vector<Event> Leader::GetUpdatesFrom(const size_t fromIndex) {
  // return events from log starting at fromIndex
  std::vector<Event> updates;

  std::lock_guard<std::mutex> lock(d_mutex);
  for (size_t i = fromIndex; i < d_events.size(); ++i) {
    updates.push_back(d_events[i]);
  }
  return updates;
}

void Leader::SubmitEvent(const Event event) {
  // add event to log and update state
  std::cout << "submitting event : " << event << std::endl;
  std::lock_guard<std::mutex> lock(d_mutex);
  d_events.push_back(event);

  if (event.d_operation == "ADD") {
    d_currValue += event.d_argument;
  } else {
    std::cerr << "Unknown operation: " << event.d_operation << std::endl;
  }
}

std::pair<int64_t, size_t> Leader::GetCurrentValueAndIndex() const {
  std::lock_guard<std::mutex> lock(d_mutex);
  return {d_currValue, d_lastWrittenIndex};
}

Event Leader::CreateRandomEvent() {
  Event event;
  event.d_operation = "ADD";
  event.d_argument = 1;
  event.d_eventIndex = d_lastWrittenIndex++;

  return event;
}

}  // namespace Calculator