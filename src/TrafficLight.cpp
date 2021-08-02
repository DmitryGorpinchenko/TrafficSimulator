#include "TrafficLight.h"

#include <random>

/* Implementation of class "MessageQueue" */

template <typename T>
MessageQueue<T>::MessageQueue(size_t max_size)
    : _max_size(max_size)
{}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    {
        std::lock_guard lock(_mutex);

        _queue.push_back(std::move(msg));
        while (_queue.size() > _max_size) {
            _queue.pop_front();
        }
    }
    _condition.notify_one();
}

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock lock(_mutex);
    _condition.wait(lock, [this]() { return !_queue.empty(); });

    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

/* Implementation of class "TrafficLight" */
 
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
    while (_msg.receive() != TrafficLightPhase::green)
        ;
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
