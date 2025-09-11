#pragma once
#include "detector.hpp"
#include <vector>

struct TrackedObject {
    int id;
    cv::Rect bbox;
    float confidence;
};

class Tracker {
public:
    Tracker();
    std::vector<TrackedObject> update(const std::vector<Detection>& detections);
private:
    int nextId;
};
