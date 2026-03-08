/**
 * @file exporter.hpp
 * @brief Export des résultats (statistiques par piste) en CSV et JSON.
 */

#pragma once
#include "analyzer.hpp"
#include <string>

/**
 * @brief Export des statistiques par ID vers des fichiers CSV et JSON.
 *
 * Écrit les champs id, distance, speed (et éventuellement d'autres) dans
 * les deux formats pour analyse ou intégration externe.
 */
class Exporter {
public:
    /**
     * @brief Construit l'exporteur avec les chemins de sortie.
     * @param csvPath Chemin du fichier CSV (ex. output/results.csv).
     * @param jsonPath Chemin du fichier JSON (ex. output/results.json).
     */
    Exporter(const std::string& csvPath, const std::string& jsonPath);

    /**
     * @brief Enregistre les statistiques dans les fichiers CSV et JSON.
     * @param results Map ID -> { distance, speed } (ex. retour de Analyzer::getResults()).
     */
    void save(const std::map<int, Stats>& results);

private:
    std::string csvPath;  /**< Chemin du fichier CSV. */
    std::string jsonPath; /**< Chemin du fichier JSON. */
};
