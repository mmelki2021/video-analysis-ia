#include "analyzer.hpp"
#include <cmath>

void Analyzer::update(const std::vector<TrackedObject>& trackedObjects) {
    for (const auto& obj : trackedObjects) {
        cv::Point center(obj.bbox.x + obj.bbox.width/2, obj.bbox.y + obj.bbox.height/2);
        if (lastPositions.find(obj.id) != lastPositions.end()) {
            float dx = center.x - lastPositions[obj.id].x;
            float dy = center.y - lastPositions[obj.id].y;
            float distance = std::sqrt(dx*dx + dy*dy);
            results[obj.id].distance += distance;
            results[obj.id].speed = distance;
        }
        lastPositions[obj.id] = center;
    }
}

std::map<int, Stats> Analyzer::getResults() const {
    return results;
}
