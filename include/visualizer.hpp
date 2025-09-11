#pragma once
#include "tracker.hpp"
#include <opencv2/opencv.hpp>

class Visualizer {
public:
    cv::Mat draw(const cv::Mat& frame, const std::vector<TrackedObject>& trackedObjects);
};
