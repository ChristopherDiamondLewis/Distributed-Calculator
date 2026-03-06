#include <grpc++/grpc++.h>
#include <shared_calculator_common.h>

#include <cmath>
#include <iostream>

namespace Calculator {
namespace Utility {
int64_t ApplyCalculation(const Event& event, int64_t currentValue) {
  // Note: We do not handle int64_t overflow/underflow for ADD, SUBTRACT,
  // MULTIPLY. For production, would add bounds checking or use arbitrary
  // precision arithmetic. Division by zero is handled below.
  int64_t result{currentValue};
  if (event.d_operation == "ADD") {
    result += event.d_argument;
  } else if (event.d_operation == "SUBTRACT") {
    result -= event.d_argument;
  } else if (event.d_operation == "MULTIPLY") {
    result *= event.d_argument;
  } else if (event.d_operation == "SQUARE_ROOT") {
    result = static_cast<int64_t>(std::sqrt(static_cast<double>(result)));
  } else if (event.d_operation == "POWER_OF_TWO") {
    result = static_cast<int64_t>(std::pow(static_cast<double>(result), 2));
  } else if (event.d_operation == "DIVIDE") {
    if (event.d_argument != 0) {
      result /= event.d_argument;
    } else {
      // Division by zero - return unchanged
      std::cout << "ERROR: Division by zero attempted, skipping event"
                << std::endl;
    }
  }
  return result;
}

void HandleGrpcStatus(const grpc::Status& status,
                      const std::string& userMessage) {
  if (not status.ok()) {
    std::cerr << "gRPC error: " << status.error_message()
              << ", error code: " << status.error_code()
              << ", user error message: " << userMessage << std::endl;
  } else {
    std::cout << userMessage << std::endl;
  }
}
}  // namespace Utility
}  // namespace Calculator
