#include <shared_calculator_leader.h>

#include <thread>
#include <chrono>


namespace Calculator {


    Leader::Leader() : d_currValue(0), d_lastWrittenIndex(0) {};

    bool Leader::Run() {
        // main loop for processing events and updating state

        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            const auto event = CreateRandomEvent();
            SubmitEvent(event);
        }

        return true;
    }


        void Leader::SubmitEvent(const Event event)
        {
            // add event to log and update state
            d_events.push_back(event);

            if(event.d_operation == "ADD")
            {
                d_currValue += event.d_argument;
            }
            else if(event.d_operation == "SUBTRACT")
            {
                d_currValue -= event.d_argument;
            }
            else
            {
                std::cerr << "Unknown operation: " << event.d_operation << std::endl;
            }
          
        }
        Event Leader::CreateRandomEvent()
        {
            // create a random event for testing
            Event event;
            event.d_operation = "ADD";
            event.d_argument = 1;
            event.d_eventIndex = d_lastWrittenIndex++;
            return event;
        }



}