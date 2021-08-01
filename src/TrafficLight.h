#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <atomic>
#include <deque>
#include <limits>
#include <condition_variable>
#include "TrafficObject.h"

template <typename T>
class MessageQueue
{
public:
    explicit MessageQueue(size_t max_size = std::numeric_limits<size_t>::max());

    void send(T &&msg);
    T receive();

private:
    std::condition_variable _condition;
    std::mutex _mutex;
    
    std::deque<T> _queue;
    size_t _max_size;
};

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