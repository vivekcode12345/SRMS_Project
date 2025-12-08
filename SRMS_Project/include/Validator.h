#pragma once
#include <string>

class Validator {
public:
    static bool validRoll(const std::string& r);
    static bool validCGPA(const std::string& s, double &out);
};

