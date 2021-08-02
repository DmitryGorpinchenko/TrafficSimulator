#include "Vehicle.h"
#include "Street.h"
#include "Intersection.h"

#include <iostream>
#include <random>

Vehicle::Vehicle()
    : TrafficObject(ObjectType::objectVehicle)
    , _posStreet(0.0)
    , _speed(400)
{}

void Vehicle::setCurrentDestination(std::shared_ptr<Intersection> destination)
{
    _currDestination = std::move(destination);
    _posStreet = 0.0;
}

void Vehicle::simulate()
{
    threads.emplace_back(&Vehicle::drive, this);
}

void Vehicle::drive()
{
    std::unique_lock<std::mutex> lck(_mtx);
    std::cout << "Vehicle #" << _id << "::drive: thread id = " << std::this_thread::get_id() << std::endl;
    lck.unlock();

    bool hasEnteredIntersection = false;
    const std::chrono::milliseconds cycleDuration(1); // duration of a single simulation cycle in ms

    auto cycleStart = std::chrono::steady_clock::now();
    while (true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - cycleStart);
        if (elapsedTime < cycleDuration) {
            continue;
        }

        _posStreet += _speed * elapsedTime.count() / 1000;

        const double completion = _posStreet / _currStreet->getLength();

        // compute current pixel position on street based on driving direction
        auto i2 = _currDestination;
        auto i1 = i2->getID() == _currStreet->getInIntersection()->getID() ? _currStreet->getOutIntersection() : _currStreet->getInIntersection();

        double x1, y1, x2, y2, xv, yv, dx, dy, l;
        i1->getPosition(x1, y1);
        i2->getPosition(x2, y2);
        dx = x2 - x1;
        dy = y2 - y1;
        l = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (x1 - x2));
        xv = x1 + completion * dx; // new position based on line equation in parameter form
        yv = y1 + completion * dy;
        setPosition(xv, yv);

        // check wether halting position in front of destination has been reached
        if (completion >= 0.9 && !hasEnteredIntersection) {
            // request entry to the current intersection and wait until entry has been granted
           _currDestination->waitForPermissionToEnter(get_shared_this());

            // slow down and set intersection flag
            _speed /= 10.0;
            hasEnteredIntersection = true;
        }

        // check wether intersection has been crossed
        if (completion >= 1.0 && hasEnteredIntersection) {
            // choose next street and destination
            const auto streetOptions = _currDestination->queryStreets(_currStreet);
            std::shared_ptr<Street> nextStreet;
            if (streetOptions.size() > 0) {
                std::random_device rd;
                std::mt19937 eng(rd());
                std::uniform_int_distribution<> distr(0, streetOptions.size() - 1);
                nextStreet = streetOptions.at(distr(eng));
            } else { // this street is a dead-end, so drive back the same way
                nextStreet = _currStreet;
            }

            // pick the one intersection at which the vehicle is currently not
            auto nextIntersection = nextStreet->getInIntersection()->getID() == _currDestination->getID() ? nextStreet->getOutIntersection() : nextStreet->getInIntersection();

            // notify _currDestination that vehicle has left BEFORE replacing it with new value right below
            _currDestination->vehicleHasLeft(get_shared_this());

            setCurrentDestination(std::move(nextIntersection));
            setCurrentStreet(std::move(nextStreet));

            // reset speed and intersection flag
            _speed *= 10.0;
            hasEnteredIntersection = false;
        }

        cycleStart = std::chrono::steady_clock::now();
    }
}
