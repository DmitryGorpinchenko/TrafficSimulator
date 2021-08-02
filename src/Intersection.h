#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "TrafficObject.h"
#include "TrafficLight.h"
#include "MessageQueue.h"

#include <vector>
#include <future>
#include <mutex>
#include <condition_variable>
#include <memory>

class Street;
class Vehicle;

struct WaitingVehicle
{
    std::shared_ptr<Vehicle> vehicle;
    std::promise<void> promise;
};

class Intersection : public TrafficObject
{
public:
    Intersection();

    std::vector<std::shared_ptr<Street>> queryStreets(std::shared_ptr<Street> incoming) const; // return list of all outgoing streets
    bool trafficLightIsGreen() const;

    void addStreet(std::shared_ptr<Street> street);

    void waitForPermissionToEnter(std::shared_ptr<Vehicle> vehicle);
    void vehicleHasLeft(std::shared_ptr<Vehicle> vehicle);

    void simulate() override;

private:
    void processVehicleQueue();

    std::vector<std::shared_ptr<Street>> _streets;   // list of all streets connected to this intersection
    TrafficLight _trafficLight;                      // traffic light of this intersection
    MessageQueue<WaitingVehicle> _waitingVehicles;   // vehicles and their associated promises waiting to enter the intersection

    std::mutex _mutex;
    std::condition_variable _cv;
    bool _isBlocked;
};

#endif
