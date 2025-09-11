#include "tracker.hpp"
#include <iostream>

Tracker::Tracker() : nextId(0) {}

std::vector<TrackedObject> Tracker::update(const std::vector<Detection>& detections) {
    std::vector<TrackedObject> trackedObjects;
    for (const auto& det : detections) {
        TrackedObject obj;
        obj.id = nextId++; // TODO: remplacer par MOT
        obj.bbox = det.bbox;
        obj.confidence = det.confidence;
        trackedObjects.push_back(obj);
    }
    return trackedObjects;
}
