/**
 * @file visualizer.hpp
 * @brief Dessin des annotations (bbox, ID) sur les frames vidéo.
 */

#pragma once
#include "tracker.hpp"
#include <opencv2/opencv.hpp>

/**
 * @brief Visualisation des objets suivis sur l'image.
 *
 * Dessine les boîtes englobantes et les identifiants pour chaque TrackedObject.
 */
class Visualizer {
public:
    /**
     * @brief Dessine les bbox et les ID sur une copie de la frame.
     * @param frame Image source (BGR).
     * @param trackedObjects Liste des objets à afficher (ID, bbox, confiance).
     * @return Nouvelle image avec rectangles et textes (frame inchangée).
     */
    cv::Mat draw(const cv::Mat& frame, const std::vector<TrackedObject>& trackedObjects);
};
