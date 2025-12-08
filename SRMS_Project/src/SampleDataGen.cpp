#include "../include/Student.h"
#include "../include/DataManager.h"
#include <iostream>

int main() {
    DataManager dm("data/students.csv");
    // simple generator of 5 sample rows
    for (int i=1;i<=5;i++){
        Student s;
        s.roll = "SRM" + std::to_string(100+i);
        s.name = "Student " + std::to_string(i);
        s.branch = "CSE";
        s.year = 1 + (i%4);
        s.cgpa = 7.0 + i*0.2;
        dm.addStudent(s);
    }
    std::cout<<"Sample data created\n";
    return 0;
}

