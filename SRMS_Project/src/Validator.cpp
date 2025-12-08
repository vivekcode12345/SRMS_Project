#include "../include/Validator.h"
#include <regex>

bool Validator::validRoll(const std::string& r) {
    if (r.empty()) return false;
    std::regex re("^[A-Za-z0-9\\-]+$");
    return std::regex_match(r, re);
}

bool Validator::validCGPA(const std::string& s, double &out) {
    try {
        out = std::stod(s);
        return out >= 0.0 && out <= 10.0;
    } catch(...) {
        return false;
    }
}

