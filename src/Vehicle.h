#ifndef VEHICLE_H
#define VEHICLE_H

#include "TrafficObject.h"
#include "MessageQueue.h"

#include <memory>

class Street;
class Intersection;

class Vehicle : public TrafficObject, public std::enable_shared_from_this<Vehicle>
{
public:
    Vehicle();

    void setCurrentStreet(std::shared_ptr<Street> street) { _currStreet = std::move(street); };
    void setCurrentDestination(std::shared_ptr<Intersection> destination);

    void notifyIntersectionEntryGranted(std::shared_ptr<Intersection> intersection);

    void simulate() override;

    std::shared_ptr<Vehicle> get_shared_this() { return shared_from_this(); }

private:
    void drive();

    std::shared_ptr<Street> _currStreet;            // street on which the vehicle is currently on
    std::shared_ptr<Intersection> _currDestination; // destination to which the vehicle is currently driving
    double _posStreet;                              // position on current street
    double _speed;                                  // ego speed in m/s

    MessageQueue<std::shared_ptr<Intersection>> _entryGrantedMsg;
};

#endif
