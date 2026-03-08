/**
 * @file tracker.hpp
 * @brief Suivi multi-objets (MOT) : association d'IDs persistants aux détections.
 */

#pragma once
#include "detector.hpp"
#include <vector>

/** Objet suivi : détection à laquelle est associé un identifiant stable entre les frames. */
struct TrackedObject {
    int id;             /**< Identifiant unique de la piste. */
    cv::Rect bbox;      /**< Boîte englobante courante. */
    float confidence;  /**< Score de confiance de la détection. */
};

/**
 * @brief Tracker associant un ID à chaque détection.
 *
 * Version simple : chaque détection reçoit un nouvel ID (pas de matching inter-frames).
 * Évolution prévue : MOT (ex. DeepSORT/ByteTrack) pour maintenir les IDs dans le temps.
 */
class Tracker {
public:
    /** @brief Construit le tracker (compteur d'IDs initialisé à 0). */
    Tracker();

    /**
     * @brief Met à jour les pistes à partir des détections de la frame courante.
     * @param detections Liste des détections (bbox, confiance, classId).
     * @return Liste des objets suivis avec ID, bbox et confiance.
     */
    std::vector<TrackedObject> update(const std::vector<Detection>& detections);

private:
    int nextId; /**< Prochain identifiant à attribuer (incrémenté à chaque nouvelle détection). */
};
