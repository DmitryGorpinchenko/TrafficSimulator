#include "Street.h"
#include "Intersection.h"

Street::Street()
    : TrafficObject(ObjectType::objectStreet)
    , _length(1000.0)
{}

void Street::setInIntersection(std::shared_ptr<Intersection> in)
{
    _interIn = std::move(in);
    _interIn->addStreet(get_shared_this()); // add this street to list of streets connected to the intersection
}

void Street::setOutIntersection(std::shared_ptr<Intersection> out)
{
    _interOut = std::move(out);
    _interOut->addStreet(get_shared_this()); // add this street to list of streets connected to the intersection
}
