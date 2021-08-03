#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "TrafficObject.h"
#include "TrafficLight.h"
#include "MessageQueue.h"

#include <vector>
#include <memory>

class Street;
class Vehicle;

class Intersection : public TrafficObject, public std::enable_shared_from_this<Intersection>
{
public:
    Intersection();

    std::vector<std::shared_ptr<Street>> queryStreets(std::shared_ptr<Street> incoming) const; // return list of all outgoing streets

    void addStreet(std::shared_ptr<Street> street);

    void addVehicleToQueue(std::shared_ptr<Vehicle> vehicle);
    void notifyVehicleLeft(std::shared_ptr<Vehicle> vehicle);

    bool trafficLightIsGreen() const;
    void waitForGreen();

    void simulate() override;

    std::shared_ptr<Intersection> get_shared_this() { return shared_from_this(); }

private:
    void processVehicleQueue();

    std::vector<std::shared_ptr<Street>> _streets;
    TrafficLight _trafficLight;
    MessageQueue<std::shared_ptr<Vehicle>> _waitingVehicles;
    MessageQueue<std::shared_ptr<Vehicle>> _vehicleLeftMsg;
};

#endif
