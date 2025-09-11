#pragma once
#include "tracker.hpp"
#include <map>
#include <vector>

struct Stats {
    float distance;
    float speed;
};

class Analyzer {
public:
    void update(const std::vector<TrackedObject>& trackedObjects);
    std::map<int, Stats> getResults() const;
private:
    std::map<int, cv::Point> lastPositions;
    std::map<int, Stats> results;
};
