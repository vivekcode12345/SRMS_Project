// src/Logger.cpp
#include "Logger.h"
#include <fstream>
#include <chrono>
#include <ctime>

Logger::Logger(const std::string &path) : logfile(path) { }

void Logger::append(const std::string &msg) {
    std::ofstream out(logfile, std::ios::app);
    if (!out.is_open()) return;

    // simple timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char timebuf[64];
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    out << "[" << timebuf << "] " << msg << "\n";
    out.close();
}
