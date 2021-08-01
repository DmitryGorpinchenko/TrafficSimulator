#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include <future>
#include <mutex>
#include <memory>
#include <atomic>
#include "TrafficObject.h"
#include "TrafficLight.h"

class Street;
class Vehicle;

// auxiliary class to queue and dequeue waiting vehicles in a thread-safe manner
class WaitingVehicles
{
public:
    int getSize() const;

    void pushBack(std::shared_ptr<Vehicle> vehicle, std::promise<void> &&promise);
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

    void setIsBlocked(bool isBlocked);

    void addVehicleToQueue(std::shared_ptr<Vehicle> vehicle);
    void addStreet(std::shared_ptr<Street> street);
    std::vector<std::shared_ptr<Street>> queryStreets(std::shared_ptr<Street> incoming); // return pointer to current list of all outgoing streets
    void simulate() override;
    void vehicleHasLeft(std::shared_ptr<Vehicle> vehicle);
    bool trafficLightIsGreen();

private:
    void processVehicleQueue();

    std::vector<std::shared_ptr<Street>> _streets;   // list of all streets connected to this intersection
    TrafficLight _trafficLight;                      // traffic light of this intersection
    WaitingVehicles _waitingVehicles;                // list of all vehicles and their associated promises waiting to enter the intersection
    std::atomic<bool> _isBlocked;                    // flag indicating wether the intersection is blocked by a vehicle
};

#endif
