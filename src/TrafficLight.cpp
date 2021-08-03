#include "TrafficLight.h"

#include <random>
 
TrafficLight::TrafficLight()
    : TrafficObject(ObjectType::objectTrafficLight)
    , _msg(1)
    , _currentPhase(TrafficLightPhase::red)
{}

TrafficLightPhase TrafficLight::getCurrentPhase() const
{
    return _currentPhase;
}

void TrafficLight::waitForGreen()
{
    _msg.waitForMessage(TrafficLightPhase::green);
}

void TrafficLight::simulate()
{
    threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

void TrafficLight::cycleThroughPhases()
{  
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dis(4000, 6000);
    const std::chrono::milliseconds cycleDuration(dis(gen));
  
    auto cycleStart = std::chrono::steady_clock::now();
    while (true) {      
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - cycleStart);
        if (elapsedTime < cycleDuration) {
            continue;
        }
        
        _currentPhase = (_currentPhase == TrafficLightPhase::green) ? TrafficLightPhase::red : TrafficLightPhase::green;
        _msg.send(TrafficLightPhase(_currentPhase));

        cycleStart = std::chrono::steady_clock::now();
    }
}
