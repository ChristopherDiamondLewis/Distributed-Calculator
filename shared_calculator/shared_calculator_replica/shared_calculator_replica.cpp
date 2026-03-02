#include <sharedCalculator.grpc.pb.h>
#include <sharedCalculator.pb.h>
#include <shared_calculator_replica.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace Calculator {

Replica::Replica(std::unique_ptr<sharedcalculator::Leader::Stub> stub)
    : d_currValue(0), d_lastIndexGotten(0), d_stub(std::move(stub)) {}

bool Replica::Run() {
  std::cout << "Polling for events" << std::endl;

  // On startup, get the most recent event and current value to resume from
  // there
  const auto mostRecentValue = GetMostRecentValue();
  if (mostRecentValue.has_value()) {
    const auto [startingValue, startingIndex] = mostRecentValue.value();
    d_currValue = startingValue;
    d_lastIndexGotten = startingIndex;
    std::cout << "Replica initialized to value: " << d_currValue
              << " at event index: " << d_lastIndexGotten << std::endl;
  }

  static constexpr int STARTING_BACKOFF_MS = 10;
  int backoffMs = STARTING_BACKOFF_MS;
  while (true) {
    const auto events = getEventsFromIndex(d_lastIndexGotten);

    if (events.empty()) {
      std::cout << "No new events, backing off for " << backoffMs << " ms"
                << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(backoffMs));
      backoffMs = backoffMs << 1;  // exponential backoff
      continue;
    }

    backoffMs = STARTING_BACKOFF_MS;

    for (const auto& event : events) {
      applyEvent(event);
    }
  }

  return true;
}

void Replica::applyEvent(const Event event) {
  std::cout << "Applying event: " << event << std::endl;
  if (event.d_operation == "ADD") {
    d_currValue += event.d_argument;
  } else {
    std::cerr << "Unknown operation: " << event.d_operation << std::endl;
  }
  std::cout << "Current value: " << d_currValue << std::endl;
  d_lastIndexGotten = event.d_eventIndex + 1;
}

std::vector<Event> Replica::getEventsFromIndex(const size_t fromIndex) const {
  std::vector<Event> events;
  grpc::ClientContext context;
  sharedcalculator::GetUpdatesResponse response;

  sharedcalculator::GetUpdatesRequest request;
  request.set_from_index(fromIndex);
  grpc::Status status = d_stub->GetUpdates(&context, request, &response);
  if (!status.ok()) {
    std::cerr << "Error getting updates from leader: " << status.error_message()
              << std::endl;
    return events;
  }

  for (const auto& event : response.events()) {
    Event replicaEvent{.d_operation = event.operation(),
                       .d_argument = event.argument(),
                       .d_eventIndex = event.eventindex()};

    events.push_back(replicaEvent);
  }

  return events;
}

std::optional<std::pair<int64_t, size_t>> Replica::GetMostRecentValue() const {
  grpc::ClientContext context;
  sharedcalculator::GetMostRecentValueResponse response;

  sharedcalculator::GetMostRecentValueRequest request;
  grpc::Status status =
      d_stub->GetMostRecentValue(&context, request, &response);

  if (!status.ok()) {
    std::cerr << "Error getting most recent value from leader: "
              << status.error_message() << std::endl;
    return std::nullopt;
  }

  return std::make_pair(response.current_value(), response.latest_index());
}

}  // namespace Calculator
