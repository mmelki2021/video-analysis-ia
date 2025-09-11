#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

struct Detection {
    cv::Rect bbox;
    float confidence;
    int classId;
};

class Detector {
public:
    Detector(const std::string& modelPath);
    std::vector<Detection> detect(const cv::Mat& frame);
private:
    std::string modelPath;
};
