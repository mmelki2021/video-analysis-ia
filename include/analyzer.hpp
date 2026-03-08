/**
 * @file analyzer.hpp
 * @brief Calcul des statistiques par piste (distance parcourue, vitesse).
 */

#pragma once
#include "tracker.hpp"
#include <map>
#include <vector>

/** Statistiques agrégées pour un objet suivi (une piste). */
struct Stats {
    float distance; /**< Distance cumulée parcourue (en pixels ou mètres selon calibration). */
    float speed;   /**< Vitesse instantanée (dernière frame) ou moyenne selon l'implémentation. */
};

/**
 * @brief Analyse des trajectoires : distance et vitesse par ID.
 *
 * Mémorise la dernière position (centre de la bbox) par ID et met à jour
 * la distance parcourue et la vitesse à chaque frame.
 */
class Analyzer {
public:
    /**
     * @brief Met à jour les statistiques à partir des objets suivis de la frame courante.
     * @param trackedObjects Liste des objets avec ID, bbox et confiance.
     */
    void update(const std::vector<TrackedObject>& trackedObjects);

    /**
     * @brief Retourne les statistiques agrégées par identifiant de piste.
     * @return Map ID -> { distance, speed }.
     */
    std::map<int, Stats> getResults() const;

private:
    std::map<int, cv::Point> lastPositions; /**< Dernière position (centre) par ID. */
    std::map<int, Stats> results;            /**< Statistiques cumulées par ID. */
};
