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

    std::vector<cv::Mat> outs;
    net.forward(outs, net.getUnconnectedOutLayersNames());
    if (outs.empty()) {
        std::cerr << "Aucune sortie du réseau DNN." << std::endl;
        return detections;
    }
    cv::Mat output = outs[0];

    const int numChannels = 84;   // 4 bbox + 80 classes (YOLOv8)
    const int numPredictions = 8400;
    if (output.dims != 3 || output.size[0] != 1 || output.size[1] != numChannels || output.size[2] != numPredictions) {
        std::cerr << "Format de sortie YOLOv8 inattendu: dims=" << output.dims;
        if (output.dims >= 1) std::cerr << " size[0]=" << output.size[0];
        if (output.dims >= 2) std::cerr << " size[1]=" << output.size[1];
        if (output.dims >= 3) std::cerr << " size[2]=" << output.size[2];
        std::cerr << std::endl;
        return detections;
    }

    // YOLOv8: (1, 84, 8400) -> transposer en (8400, 84) pour une ligne = une détection
    cv::Mat out(numPredictions, numChannels, CV_32F);
    const float* src = output.ptr<float>();
    for (int i = 0; i < numPredictions; ++i) {
        for (int c = 0; c < numChannels; ++c) {
            out.at<float>(i, c) = src[c * numPredictions + i];
        }
    }

    float scaleX = static_cast<float>(frame.cols) / inputWidth;
    float scaleY = static_cast<float>(frame.rows) / inputHeight;

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < out.rows; ++i) {
        // YOLOv8: pas d'objectness séparé, confiance = max des scores de classe
        cv::Mat scores = out.row(i).colRange(4, out.cols);
        cv::Point classIdPoint;
        double maxClassScore;
        cv::minMaxLoc(scores, nullptr, &maxClassScore, nullptr, &classIdPoint);
        float confidence = static_cast<float>(maxClassScore);
        if (confidence < confThreshold) {
            continue;
        }

        float cx = out.at<float>(i, 0);
        float cy = out.at<float>(i, 1);
        float w = out.at<float>(i, 2);
        float h = out.at<float>(i, 3);
        // Coordonnées dans l'image 640x640 -> ramener à la taille de la frame
        int left = static_cast<int>((cx - w / 2.0f) * scaleX);
        int top = static_cast<int>((cy - h / 2.0f) * scaleY);
        int width = static_cast<int>(w * scaleX);
        int height = static_cast<int>(h * scaleY);

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
