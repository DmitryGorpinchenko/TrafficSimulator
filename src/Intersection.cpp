#include "Intersection.h"
#include "Street.h"
#include "Vehicle.h"

#include <iostream>
#include <chrono>

Intersection::Intersection()
    : TrafficObject(ObjectType::objectIntersection)
    , _vehicleLeftMsg(1)
{}

std::vector<std::shared_ptr<Street>> Intersection::queryStreets(std::shared_ptr<Street> incoming) const
{
    std::vector<std::shared_ptr<Street>> outgoings;
    for (const auto &it : _streets) {
        if (incoming->getID() != it->getID()) {
            outgoings.push_back(it);
        }
    }
    return outgoings;
}

void Intersection::addStreet(std::shared_ptr<Street> street)
{
    _streets.push_back(std::move(street));
}

void Intersection::addVehicleToQueue(std::shared_ptr<Vehicle> vehicle)
{
    _waitingVehicles.send(std::move(vehicle));
}

void Intersection::notifyVehicleLeft(std::shared_ptr<Vehicle> vehicle)
{
    _vehicleLeftMsg.send(std::move(vehicle));
}

bool Intersection::trafficLightIsGreen() const
{
    return _trafficLight.getCurrentPhase() == TrafficLightPhase::green;
}

void Intersection::waitForGreen()
{
    _trafficLight.waitForGreen();
}

void Intersection::simulate()
{
    _trafficLight.simulate();
    threads.emplace_back(&Intersection::processVehicleQueue, this);
}

void Intersection::processVehicleQueue()
{
    while (true) {
        auto wv = _waitingVehicles.receive();
        wv->notifyIntersectionEntryGranted(get_shared_this());
        _vehicleLeftMsg.waitForMessage(wv);
    }
}
