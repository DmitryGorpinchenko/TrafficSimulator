#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "TrafficObject.h"
#include "TrafficLight.h"

#include <vector>
#include <future>
#include <mutex>
#include <memory>
#include <atomic>

class Street;
class Vehicle;

// auxiliary class to queue and dequeue waiting vehicles in a thread-safe manner
class WaitingVehicles
{
public:
    int getSize() const;

    std::future<void> pushBack(std::shared_ptr<Vehicle> vehicle);
    void permitEntryToFirstInQueue();

private:
    mutable std::mutex _mutex;

    std::vector<std::shared_ptr<Vehicle>> _vehicles; // list of all vehicles waiting to enter corresponding intersection
    std::vector<std::promise<void>> _promises;       // list of associated promises
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

    void setIsBlocked(bool isBlocked);

    std::vector<std::shared_ptr<Street>> _streets;   // list of all streets connected to this intersection
    TrafficLight _trafficLight;                      // traffic light of this intersection
    WaitingVehicles _waitingVehicles;                // list of all vehicles and their associated promises waiting to enter the intersection
    std::atomic<bool> _isBlocked;                    // flag indicating wether the intersection is blocked by a vehicle
};

#endif
