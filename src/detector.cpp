#include "detector.hpp"
#include <iostream>

Detector::Detector(const std::string& modelPath)
    : modelPath(modelPath),
      inputWidth(640),
      inputHeight(640),
      confThreshold(0.4f),
      nmsThreshold(0.45f) {
    std::cout << "Initialisation du détecteur avec modèle: " << modelPath << std::endl;
    try {
        net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    } catch (const cv::Exception& e) {
        std::cerr << "Erreur lors du chargement du modèle ONNX: " << e.what() << std::endl;
        throw;
    }
}

std::vector<Detection> Detector::detect(const cv::Mat& frame) {
    std::vector<Detection> detections;

    if (frame.empty() || net.empty()) {
        return detections;
    }

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

    net.setInput(blob);

    cv::Mat output = net.forward();
    cv::Mat out;

    if (output.dims == 3) {
        int rows = output.size[2];
        int cols = output.size[1];
        out = cv::Mat(rows, cols, CV_32F, output.ptr<float>()).clone();
    } else if (output.dims == 2) {
        out = output;
    } else {
        std::cerr << "Format de sortie DNN inattendu (dims=" << output.dims << ")" << std::endl;
        return detections;
    }

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < out.rows; ++i) {
        float objConf = out.at<float>(i, 4);
        if (objConf < confThreshold) {
            continue;
        }

        cv::Mat scores = out.row(i).colRange(5, out.cols);
        cv::Point classIdPoint;
        double maxClassScore;
        cv::minMaxLoc(scores, nullptr, &maxClassScore, nullptr, &classIdPoint);

        float confidence = objConf * static_cast<float>(maxClassScore);
        if (confidence < confThreshold) {
            continue;
        }

        float cx = out.at<float>(i, 0);
        float cy = out.at<float>(i, 1);
        float w = out.at<float>(i, 2);
        float h = out.at<float>(i, 3);

        int left = static_cast<int>(cx - w / 2.0f);
        int top = static_cast<int>(cy - h / 2.0f);
        int width = static_cast<int>(w);
        int height = static_cast<int>(h);

        boxes.emplace_back(left, top, width, height);
        confidences.push_back(confidence);
        classIds.push_back(classIdPoint.x);
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    for (int idx : indices) {
        Detection det;
        cv::Rect clippedBox = boxes[idx] & cv::Rect(0, 0, frame.cols, frame.rows);
        det.bbox = clippedBox;
        det.confidence = confidences[idx];
        det.classId = classIds[idx];
        detections.push_back(det);
    }

    return detections;
}
