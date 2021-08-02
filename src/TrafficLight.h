#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include "MessageQueue.h"

#include <atomic>

enum class TrafficLightPhase { red, green };

class TrafficLight : public TrafficObject
{
public:
    TrafficLight();

    TrafficLightPhase getCurrentPhase() const;

    void waitForGreen();
    void simulate() override;

private:
    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _msg;
    std::atomic<TrafficLightPhase> _currentPhase;
};

#endif