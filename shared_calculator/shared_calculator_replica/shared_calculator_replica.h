#pragma once

#include <sharedCalc.grpc.pb.h>

#include <string>

namespace Calculator {

struct Event {
  std::string d_operation;
  int64_t d_argument{0};
  size_t d_eventIndex{0};
};

class Replica {
 public:
  // constructor, destructor, and other member functions
  Replica();
  ~Replica();

  bool Run();
  Event getEvent(const size_t eventIndex);

 private:
  int64_t d_currValue;
  size_t d_lastIndexGotten;
};
}  // namespace Calculator