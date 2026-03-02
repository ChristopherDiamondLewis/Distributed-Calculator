#pragma once

#include <sharedCalculator.grpc.pb.h>
#include <sharedCalculator.pb.h>

#include <string>
#include <vector>

namespace Calculator {

struct Event {
  std::string d_operation;
  int64_t d_argument{0};
  size_t d_eventIndex{0};

  friend std::ostream& operator<<(std::ostream& os, const Event& event) {
    os << event.d_operation << " " << event.d_argument;
    return os;
  }
};

class Leader {
 public:
  // constructor, destructor, and other member functions
  Leader();
  ~Leader() = default;

  bool Run();
  void SubmitEvent(const Event event);
  Event CreateRandomEvent();
  std::vector<Event> GetUpdatesFrom(const size_t fromIndex);

 private:
  int64_t d_currValue;
  std::vector<Event> d_events;
  size_t d_lastWrittenIndex;
};
}  // namespace Calculator