#include <iostream>
#include <opencv2/opencv.hpp>
#include "detector.hpp"
#include "tracker.hpp"
#include "analyzer.hpp"
#include "visualizer.hpp"
#include "exporter.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./video_analysis <video_path>" << std::endl;
        return -1;
    }

    std::string videoPath = argv[1];
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Erreur : impossible d'ouvrir la vidéo " << videoPath << std::endl;
        return -1;
    }

    Detector detector("models/yolov8.onnx");
    Tracker tracker;
    Analyzer analyzer;
    Visualizer visualizer;
    Exporter exporter("output/results.csv", "output/results.json");

    cv::Mat frame;
    while (cap.read(frame)) {
        auto detections = detector.detect(frame);
        auto trackedObjects = tracker.update(detections);
        analyzer.update(trackedObjects);
        cv::Mat outputFrame = visualizer.draw(frame, trackedObjects);
        cv::imshow("Video Analysis", outputFrame);
        if (cv::waitKey(1) == 27) break;
    }

    exporter.save(analyzer.getResults());
    return 0;
}
