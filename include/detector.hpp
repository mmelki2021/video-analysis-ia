#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>

#ifdef USE_ONNXRUNTIME
#include <onnxruntime_cxx_api.h>
#endif

#ifndef USE_ONNXRUNTIME
#include <opencv2/dnn.hpp>
#endif

struct Detection {
    cv::Rect bbox;
    float confidence;
    int classId;
};

class Detector {
public:
    Detector(const std::string& modelPath);
    ~Detector();
    std::vector<Detection> detect(const cv::Mat& frame);
private:
    std::string modelPath;
    int inputWidth;
    int inputHeight;
    float confThreshold;
    float nmsThreshold;

#ifdef USE_ONNXRUNTIME
    Ort::Env env;
    Ort::SessionOptions sessionOptions;
    std::unique_ptr<Ort::Session> session;
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    std::vector<const char*> inputNamesPtr;
    std::vector<const char*> outputNamesPtr;
#else
    cv::dnn::Net net;
#endif
};
