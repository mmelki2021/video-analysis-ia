/**
 * @file detector.hpp
 * @brief Détection d'objets (personnes) via YOLOv8 ONNX (backend OpenCV DNN ou ONNX Runtime).
 */

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

/** Résultat d'une détection : boîte englobante, confiance et identifiant de classe (COCO). */
struct Detection {
    cv::Rect bbox;   /**< Boîte englobante dans l'image. */
    float confidence; /**< Score de confiance dans [0, 1]. */
    int classId;     /**< Indice de classe (ex. 0 = personne pour COCO). */
};

/**
 * @brief Détecteur YOLOv8 ONNX (double backend : ONNX Runtime ou OpenCV DNN).
 *
 * Charge un modèle ONNX (ex. models/yolov8n.onnx), prétraite les frames (blob 640×640),
 * exécute l'inférence et retourne les détections après NMS.
 */
class Detector {
public:
    /**
     * @brief Construit le détecteur et charge le modèle ONNX.
     * @param modelPath Chemin vers le fichier .onnx (ex. models/yolov8n.onnx).
     * @throws cv::Exception si le chargement échoue (OpenCV DNN) ou std::exception (ONNX Runtime).
     */
    Detector(const std::string& modelPath);
    /** @brief Destructeur. */
    ~Detector();

    /**
     * @brief Exécute la détection sur une frame.
     * @param frame Image BGR (vidéo ou webcam).
     * @return Liste des détections (bbox, confiance, classId) après NMS.
     */
    std::vector<Detection> detect(const cv::Mat& frame);

private:
    std::string modelPath;   /**< Chemin du fichier ONNX. */
    int inputWidth;          /**< Largeur d'entrée du réseau (ex. 640). */
    int inputHeight;        /**< Hauteur d'entrée du réseau (ex. 640). */
    float confThreshold;   /**< Seuil minimal de confiance pour garder une détection. */
    float nmsThreshold;    /**< Seuil IoU pour la suppression non-maximale. */

#ifdef USE_ONNXRUNTIME
    Ort::Env env;
    Ort::SessionOptions sessionOptions;
    std::unique_ptr<Ort::Session> session;
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    std::vector<const char*> inputNamesPtr;
    std::vector<const char*> outputNamesPtr;
#else
    cv::dnn::Net net;       /**< Réseau OpenCV DNN (si USE_ONNXRUNTIME non défini). */
#endif
};
