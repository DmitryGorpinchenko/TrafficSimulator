#include "TrafficObject.h"

int TrafficObject::_idCnt = 0;
std::mutex TrafficObject::_mtx;

TrafficObject::TrafficObject(ObjectType type)
    : _type(type)
    , _id(_idCnt++)
{}

TrafficObject::~TrafficObject()
{
    for (auto &t : threads) {
        t.join();
    }
}

void TrafficObject::setPosition(double x, double y)
{
    _posX = x;
    _posY = y;
}

void TrafficObject::getPosition(double &x, double &y)
{
    x = _posX;
    y = _posY;
}