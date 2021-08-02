#include "Intersection.h"
#include "Street.h"
#include "Vehicle.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <future>

/* Implementation of class "WaitingVehicles" */

int WaitingVehicles::getSize() const
{
    std::lock_guard lock(_mutex);
    
    return _vehicles.size();
}

std::future<void> WaitingVehicles::pushBack(std::shared_ptr<Vehicle> vehicle)
{
    std::lock_guard lock(_mutex);

    _vehicles.push_back(std::move(vehicle));
    _promises.push_back(std::promise<void>());

    return _promises.back().get_future();
}

void WaitingVehicles::permitEntryToFirstInQueue()
{
    std::lock_guard lock(_mutex);

    auto firstPromise = _promises.begin();
    auto firstVehicle = _vehicles.begin();

    firstPromise->set_value(); // fulfill promise and send signal back that permission to enter has been granted

    _vehicles.erase(firstVehicle);
    _promises.erase(firstPromise);
}

/* Implementation of class "Intersection" */

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
    std::unique_lock<std::mutex> lock(_mtx);
    std::cout << "Intersection #" << _id << "::waitForPermissionToEnter: thread id = " << std::this_thread::get_id() << std::endl;
    lock.unlock();

    // wait until the vehicle is allowed to enter
    auto ftr = _waitingVehicles.pushBack(vehicle);
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
    // unblock queue processing
    setIsBlocked(false);
}

void Intersection::simulate()
{
    _trafficLight.simulate();
    threads.emplace_back(&Intersection::processVehicleQueue, this);
}

void Intersection::processVehicleQueue()
{
    while (true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // only proceed when at least one vehicle is waiting in the queue
        if (_waitingVehicles.getSize() > 0 && !_isBlocked) {
            // set intersection to "blocked" to prevent other vehicles from entering
            setIsBlocked(true);
            _waitingVehicles.permitEntryToFirstInQueue();
        }
    }
}

void Intersection::setIsBlocked(bool isBlocked)
{
    _isBlocked = isBlocked;
}