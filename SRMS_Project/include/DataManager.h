#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <vector>
#include <string>
#include "Student.h"

// Struct for Dashboard Stats
struct StudentStats {
    int totalCount;
    double avgCGPA;
    std::string topStudent;
};

class DataManager {
public:
    DataManager(const std::string &path = "data/students.csv");

    bool loadAll();
    bool saveAll();
    bool importCSV(const std::string &path);
    bool exportCSV(const std::string &path);

    // --- NEW FEATURES ---
    bool existsRoll(const std::string& roll); // For Duplicate Check
    void sortStudents(int columnIndex, bool ascending); // For Sorting
    StudentStats getStats(); // For Dashboard
    std::vector<Student> search(const std::string& query); // For Search
    // --------------------

    int nextId();
    bool addStudent(const Student &s);
    Student* findById(int id);
    bool deleteStudent(int id);
    std::vector<Student> listAll();

    std::string dataPath;

private:
    std::vector<Student> records;
};

#endif // DATAMANAGER_H
