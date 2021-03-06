#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "TrafficObject.h"

#include <string>
#include <vector>
#include <opencv2/core.hpp>

class Graphics
{
public:
    void setBgFilename(std::string filename) { _bgFilename = std::move(filename); }
    void setTrafficObjects(std::vector<std::shared_ptr<TrafficObject>> trafficObjects) { _trafficObjects = std::move(trafficObjects); };

    void simulate();

private:
    void loadBackgroundImg();
    void drawTrafficObjects();

    std::vector<std::shared_ptr<TrafficObject>> _trafficObjects;
    std::string _bgFilename;
    std::string _windowName;
    std::vector<cv::Mat> _images;
};

#endif