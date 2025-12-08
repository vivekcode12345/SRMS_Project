#ifndef STUDENT_H
#define STUDENT_H

#include <string>

struct Student {
    int id = 0;
    std::string roll;
    std::string name;
    std::string branch;
    int year = 1;
    double cgpa = 0.0;
    bool active = true;
};

#endif // STUDENT_H
