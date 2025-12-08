#include "../include/DataManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    DataManager dm("data/students.csv");
    dm.loadAll();
    auto all = dm.listAll();
    // create new vector without inactive
    std::vector<Student> keep;
    for (auto &s: all) if (s.active) keep.push_back(s);
    // overwrite file by saving via DataManager
    // fallback: write manually
    std::ofstream out("data/students.csv", std::ios::trunc);
    for (auto &s: keep) {
        out << s.id << "," << s.roll << "," << s.name << "," << s.branch << "," << s.year << "," << s.cgpa << "," << (s.active?"1":"0") << "\n";
    }
    std::cout<<"Compaction done\n";
    return 0;
}

