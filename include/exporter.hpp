#pragma once
#include "analyzer.hpp"
#include <string>

class Exporter {
public:
    Exporter(const std::string& csvPath, const std::string& jsonPath);
    void save(const std::map<int, Stats>& results);
private:
    std::string csvPath;
    std::string jsonPath;
};
