#include <iostream>
#include <string>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <opencv2/opencv.hpp>
#include "detector.hpp"
#include "tracker.hpp"
#include "analyzer.hpp"
#include "visualizer.hpp"
#include "exporter.hpp"

static std::string resolveModelPath(const std::string& modelPath) {
    struct stat st;
    if (stat(modelPath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
        return modelPath;
    }
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len > 0) {
        exePath[len] = '\0';
        std::string exeDir(exePath);
        size_t lastSlash = exeDir.find_last_of('/');
        if (lastSlash != std::string::npos) {
            exeDir = exeDir.substr(0, lastSlash);
            std::string alt = exeDir + "/../" + modelPath;
            if (stat(alt.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                return alt;
            }
        }
    }
    return modelPath;
}

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

    std::string modelPath = resolveModelPath("models/yolov8n.onnx");
    Detector detector(modelPath);
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
