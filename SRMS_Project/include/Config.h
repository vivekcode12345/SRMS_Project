#pragma once
#include <string>

struct Config {
    std::string theme = "light";
    int rowsPerPage = 20;
    static Config load(const std::string& path = "data/settings.ini");
    static void save(const Config& cfg, const std::string& path = "data/settings.ini");
};

