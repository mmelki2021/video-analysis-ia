#include "detector.hpp"
#include <iostream>

#ifdef USE_ONNXRUNTIME
Detector::Detector(const std::string& modelPath)
    : modelPath(modelPath),
      inputWidth(640),
      inputHeight(640),
      confThreshold(0.4f),
      nmsThreshold(0.45f),
      env(ORT_LOGGING_LEVEL_WARNING, "VideoAnalysis") {
    std::cout << "Initialisation du détecteur (ONNX Runtime) avec modèle: " << modelPath << std::endl;
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
#ifdef _WIN32
    std::wstring wpath(modelPath.begin(), modelPath.end());
    session = std::make_unique<Ort::Session>(env, wpath.c_str(), sessionOptions);
#else
    session = std::make_unique<Ort::Session>(env, modelPath.c_str(), sessionOptions);
#endif
    Ort::AllocatorWithDefaultOptions allocator;
    {
        Ort::AllocatedStringPtr name = session->GetInputNameAllocated(0, allocator);
        inputNames.push_back(name.get());
    }
    {
        Ort::AllocatedStringPtr name = session->GetOutputNameAllocated(0, allocator);
        outputNames.push_back(name.get());
    }
    for (const auto& s : inputNames) inputNamesPtr.push_back(s.c_str());
    for (const auto& s : outputNames) outputNamesPtr.push_back(s.c_str());
}

Detector::~Detector() = default;
#else
Detector::Detector(const std::string& modelPath)
    : modelPath(modelPath),
      inputWidth(640),
      inputHeight(640),
      confThreshold(0.4f),
      nmsThreshold(0.45f) {
    std::cout << "Initialisation du détecteur (OpenCV DNN) avec modèle: " << modelPath << std::endl;
    try {
        net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    } catch (const cv::Exception& e) {
        std::cerr << "Erreur lors du chargement du modèle ONNX: " << e.what() << std::endl;
        std::cerr << "Conseil: compilez avec -DONNXRUNTIME_ROOT=/chemin/vers/onnxruntime pour utiliser ONNX Runtime." << std::endl;
        throw;
    }
}

Detector::~Detector() = default;
#endif

std::vector<Detection> Detector::detect(const cv::Mat& frame) {
    std::vector<Detection> detections;
    if (frame.empty()) return detections;

    cv::Mat blob;
    cv::dnn::blobFromImage(
        frame,
        blob,
        1.0 / 255.0,
        cv::Size(inputWidth, inputHeight),
        cv::Scalar(),
        true,
        false
    );

    const int numChannels = 84;
    const int numPredictions = 8400;
    float scaleX = static_cast<float>(frame.cols) / inputWidth;
    float scaleY = static_cast<float>(frame.rows) / inputHeight;

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

#ifdef USE_ONNXRUNTIME
    if (!session) return detections;
    const int64_t inputShape[] = {1, 3, inputHeight, inputWidth};
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memoryInfo,
        blob.ptr<float>(),
        blob.total(),
        inputShape,
        4
    );
    auto outputTensors = session->Run(
        Ort::RunOptions{nullptr},
        inputNamesPtr.data(),
        &inputTensor,
        1,
        outputNamesPtr.data(),
        1
    );
    const float* output = outputTensors[0].GetTensorData<float>();
    auto shape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
    if (shape.size() != 3 || shape[0] != 1 || shape[1] != numChannels || shape[2] != numPredictions) {
        std::cerr << "Format de sortie YOLOv8 inattendu." << std::endl;
        return detections;
    }
    for (int i = 0; i < numPredictions; ++i) {
        float cx = output[0 * numPredictions + i];
        float cy = output[1 * numPredictions + i];
        float w = output[2 * numPredictions + i];
        float h = output[3 * numPredictions + i];
        float maxScore = 0.f;
        int maxClass = 0;
        for (int c = 4; c < numChannels; ++c) {
            float s = output[c * numPredictions + i];
            if (s > maxScore) { maxScore = s; maxClass = c - 4; }
        }
        if (maxScore < confThreshold) continue;
        int left = static_cast<int>((cx - w / 2.0f) * scaleX);
        int top = static_cast<int>((cy - h / 2.0f) * scaleY);
        int width = static_cast<int>(w * scaleX);
        int height = static_cast<int>(h * scaleY);
        boxes.emplace_back(left, top, width, height);
        confidences.push_back(maxScore);
        classIds.push_back(maxClass);
    }
#else
    if (net.empty()) return detections;
    net.setInput(blob);
    cv::Mat output = net.forward();
    if (output.empty()) return detections;
    int outDims = output.dims;
    int rows = (outDims >= 2) ? output.size[outDims - 2] : 0;
    int cols = (outDims >= 2) ? output.size[outDims - 1] : 0;
    if (outDims == 3 && output.size[0] == 1) {
        rows = output.size[2];
        cols = output.size[1];
    } else if (outDims == 2) {
        rows = output.rows;
        cols = output.cols;
    }
    if (rows != numPredictions || cols != numChannels) {
        std::cerr << "Format de sortie YOLOv8 inattendu: " << rows << "x" << cols << std::endl;
        return detections;
    }
    const float* src = output.ptr<float>();
    for (int i = 0; i < numPredictions; ++i) {
        float cx = src[0 * numPredictions + i];
        float cy = src[1 * numPredictions + i];
        float w = src[2 * numPredictions + i];
        float h = src[3 * numPredictions + i];
        float maxScore = 0.f;
        int maxClass = 0;
        for (int c = 4; c < numChannels; ++c) {
            float s = src[c * numPredictions + i];
            if (s > maxScore) { maxScore = s; maxClass = c - 4; }
        }
        if (maxScore < confThreshold) continue;
        int left = static_cast<int>((cx - w / 2.0f) * scaleX);
        int top = static_cast<int>((cy - h / 2.0f) * scaleY);
        int width = static_cast<int>(w * scaleX);
        int height = static_cast<int>(h * scaleY);
        boxes.emplace_back(left, top, width, height);
        confidences.push_back(maxScore);
        classIds.push_back(maxClass);
    }
#endif

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    for (int idx : indices) {
        Detection det;
        det.bbox = boxes[idx] & cv::Rect(0, 0, frame.cols, frame.rows);
        det.confidence = confidences[idx];
        det.classId = classIds[idx];
        detections.push_back(det);
    }
    return detections;
}
