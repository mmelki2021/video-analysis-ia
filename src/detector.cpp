#include "detector.hpp"
#include <iostream>

Detector::Detector(const std::string& modelPath) : modelPath(modelPath) {
    std::cout << "Initialisation du détecteur avec modèle: " << modelPath << std::endl;
}

std::vector<Detection> Detector::detect(const cv::Mat& frame) {
    std::vector<Detection> detections;
    // TODO: Implémenter la détection avec DNN OpenCV
    return detections;
}
