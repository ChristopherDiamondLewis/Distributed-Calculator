#pragma once 

#include <sharedCalculator.pb.h>
#include <sharedCalculator.grpc.pb.h>

#include <vector>
#include <string>

namespace Calculator {

struct Event {
    std::string d_operation;
    int64_t d_argument{0};
    size_t d_eventIndex{0};
};

    class Leader {
    public:
        // constructor, destructor, and other member functions
        Leader();
        ~Leader();

        bool Run();
        void SubmitEvent(const Event event);
        Event CreateEvent();
        Event CreateRandomEvent();
        sharedcalculator::GetUpdatesResponse GetUpdatesFrom(const size_t fromIndex);

    private: 
        int64_t d_currValue;
        std::vector<Event> d_events;
        size_t d_lastWrittenIndex;
    };
}