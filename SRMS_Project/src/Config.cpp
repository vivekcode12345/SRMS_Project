#include "../include/Config.h"
#include <fstream>
#include <sstream>

Config Config::load(const std::string& path) {
    Config c;
    std::ifstream in(path);
    if (!in.is_open()) return c;
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("theme=",0)==0) c.theme = line.substr(6);
        if (line.rfind("rowsPerPage=",0)==0) c.rowsPerPage = std::stoi(line.substr(12));
    }
    return c;
}

void Config::save(const Config& cfg, const std::string& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) return;
    out << "theme=" << cfg.theme << "\n";
    out << "rowsPerPage=" << cfg.rowsPerPage << "\n";
}

