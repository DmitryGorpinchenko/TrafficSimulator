#include "Intersection.h"
#include "Street.h"
#include "Vehicle.h"

#include <iostream>
#include <chrono>

Intersection::Intersection()
    : TrafficObject(ObjectType::objectIntersection)
    , _isBlocked(false)
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

bool Intersection::trafficLightIsGreen() const
{
    return _trafficLight.getCurrentPhase() == TrafficLightPhase::green;
}

void Intersection::addStreet(std::shared_ptr<Street> street)
{
    _streets.push_back(std::move(street));
}

void Intersection::waitForPermissionToEnter(std::shared_ptr<Vehicle> vehicle)
{
    std::unique_lock lock(_mtx);
    std::cout << "Intersection #" << _id << "::waitForPermissionToEnter: thread id = " << std::this_thread::get_id() << std::endl;
    lock.unlock();

    // wait until the vehicle is allowed to enter
    WaitingVehicle wv = {vehicle, std::promise<void>()};
    auto ftr = wv.promise.get_future();
    _waitingVehicles.send(std::move(wv));
    ftr.wait();
    if (!trafficLightIsGreen()) {
        _trafficLight.waitForGreen();
    }
  
    lock.lock();
    std::cout << "Intersection #" << _id << ": Vehicle #" << vehicle->getID() << " is granted entry." << std::endl;
    lock.unlock();
}

void Intersection::vehicleHasLeft(std::shared_ptr<Vehicle> vehicle)
{
    { // unblock queue processing
        std::lock_guard lock(_mutex);
        _isBlocked = false;
    }
    _cv.notify_one();
}

void Intersection::simulate()
{
    _trafficLight.simulate();
    threads.emplace_back(&Intersection::processVehicleQueue, this);
}

void Intersection::processVehicleQueue()
{
    while (true) {
        std::unique_lock lock(_mutex);
        _cv.wait(lock, [this]() { return !_isBlocked; });
        lock.unlock();

        auto wv = _waitingVehicles.receive();
        lock.lock();
        _isBlocked = true; // set intersection to "blocked" to prevent other vehicles from entering
        lock.unlock();

        wv.promise.set_value();
    }
}
