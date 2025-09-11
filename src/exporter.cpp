#include "exporter.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

Exporter::Exporter(const std::string& csvPath, const std::string& jsonPath)
    : csvPath(csvPath), jsonPath(jsonPath) {}

void Exporter::save(const std::map<int, Stats>& results) {
    std::ofstream csvFile(csvPath);
    csvFile << "ID,Distance,Speed\n";
    for (const auto& [id, stats] : results) {
        csvFile << id << "," << stats.distance << "," << stats.speed << "\n";
    }
    csvFile.close();

    nlohmann::json j;
    for (const auto& [id, stats] : results) {
        j[std::to_string(id)] = {{"distance", stats.distance}, {"speed", stats.speed}};
    }
    std::ofstream jsonFile(jsonPath);
    jsonFile << j.dump(4);
    jsonFile.close();

    std::cout << "Résultats exportés dans " << csvPath << " et " << jsonPath << std::endl;
}
