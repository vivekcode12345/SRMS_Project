#include "DataManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>    // Required for tolower

DataManager::DataManager(const std::string &path) : dataPath(path) {
    // nothing
}

bool DataManager::loadAll() {
    records.clear();
    std::ifstream in(dataPath);
    if (!in.is_open()) return false;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        Student s;
        // id
        if (!std::getline(ss, token, ',')) continue;
        s.id = atoi(token.c_str());
        // roll
        if (!std::getline(ss, token, ',')) continue;
        s.roll = token;
        // name
        if (!std::getline(ss, token, ',')) continue;
        s.name = token;
        // branch
        if (!std::getline(ss, token, ',')) continue;
        s.branch = token;
        // year
        if (!std::getline(ss, token, ',')) continue;
        s.year = atoi(token.c_str());
        // cgpa
        if (!std::getline(ss, token, ',')) continue;
        s.cgpa = atof(token.c_str());
        // active
        if (!std::getline(ss, token, ',')) {
            // maybe end of line; try remaining
            token = "1";
        }
        s.active = (atoi(token.c_str()) != 0);
        records.push_back(s);
    }
    in.close();
    return true;
}

bool DataManager::saveAll() {
    std::ofstream out(dataPath, std::ios::trunc);
    if (!out.is_open()) return false;
    for (auto &s : records) {
        out << s.id << "," << s.roll << "," << s.name << "," << s.branch << ","
            << s.year << "," << s.cgpa << "," << (s.active ? 1 : 0) << "\n";
    }
    out.close();
    return true;
}

bool DataManager::importCSV(const std::string &path) {
    std::ifstream in(path);
    if (!in.is_open()) return false;
    std::string line;

    // Get the next valid ID to start assigning
    int nid = nextId();

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        Student s;
        // count commas to guess format
        int commaCount = 0;
        for (char c : line) if (c == ',') ++commaCount;

        if (commaCount >= 6) {
            // parse with id present
            std::getline(ss, token, ','); s.id = atoi(token.c_str());
            std::getline(ss, token, ','); s.roll = token;
            std::getline(ss, token, ','); s.name = token;
            std::getline(ss, token, ','); s.branch = token;
            std::getline(ss, token, ','); s.year = atoi(token.c_str());
            std::getline(ss, token, ','); s.cgpa = atof(token.c_str());
            if (!std::getline(ss, token, ',')) token = "1";
            s.active = (atoi(token.c_str()) != 0);
        } else {
            // parse without id (auto-assign)
            s.id = nid++;
            std::getline(ss, token, ','); s.roll = token;
            std::getline(ss, token, ','); s.name = token;
            std::getline(ss, token, ','); s.branch = token;
            std::getline(ss, token, ','); s.year = atoi(token.c_str());
            std::getline(ss, token, ','); s.cgpa = atof(token.c_str());
            s.active = true;
        }

        // If ID collides (from file or generated), assign new ID
        bool coll = false;
        for (auto &r: records) if (r.id == s.id) { coll = true; break; }
        if (coll) s.id = nid++;

        records.push_back(s);
    }

    in.close();
    return saveAll(); // Ensure imports are saved immediately
}

bool DataManager::exportCSV(const std::string &path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) return false;
    for (auto &s : records) {
        out << s.id << "," << s.roll << "," << s.name << "," << s.branch << ","
            << s.year << "," << s.cgpa << "," << (s.active ? 1 : 0) << "\n";
    }
    out.close();
    return true;
}

int DataManager::nextId() {
    int maxid = 0;
    for (auto &s : records) if (s.id > maxid) maxid = s.id;
    return maxid + 1;
}

// --- CORRECTED FUNCTION ---
bool DataManager::addStudent(const Student &s) {
    Student newStudent = s;  // Create a copy of the input student
    newStudent.id = nextId(); // Assign the next unique ID
    newStudent.active = true; // Ensure the student is marked active

    records.push_back(newStudent); // Add to memory
    return saveAll();              // Save to disk immediately
}
// --------------------------

Student* DataManager::findById(int id) {
    for (auto &s : records) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

bool DataManager::deleteStudent(int id) {
    for (auto &s : records) {
        if (s.id == id) {
            s.active = false;
            return saveAll(); // Save changes to disk
        }
    }
    return false;
}

std::vector<Student> DataManager::listAll() {
    return records;
}

// 1. DUPLICATE CHECK
bool DataManager::existsRoll(const std::string& roll) {
    for (const auto& s : records) {
        if (s.active && s.roll == roll) return true;
    }
    return false;
}

// 2. SORTING
void DataManager::sortStudents(int columnIndex, bool ascending) {
    std::sort(records.begin(), records.end(), [columnIndex, ascending](const Student& a, const Student& b) {
        bool result = false;
        switch (columnIndex) {
            case 0: result = a.id < b.id; break;       // ID
            case 1: result = a.roll < b.roll; break;   // Roll
            case 2: result = a.name < b.name; break;   // Name
            case 3: result = a.branch < b.branch; break;// Branch
            case 4: result = a.year < b.year; break;   // Year
            case 5: result = a.cgpa < b.cgpa; break;   // CGPA
            default: return false;
        }
        return ascending ? result : !result;
    });
}

// 3. SEARCH
std::vector<Student> DataManager::search(const std::string& query) {
    if (query.empty()) return listAll();

    std::vector<Student> result;
    std::string qLower = query;
    // Convert query to lowercase for case-insensitive search
    std::transform(qLower.begin(), qLower.end(), qLower.begin(), ::tolower);

    for (const auto& s : records) {
        if (!s.active) continue;

        std::string nLower = s.name;
        std::string rLower = s.roll;
        std::transform(nLower.begin(), nLower.end(), nLower.begin(), ::tolower);
        std::transform(rLower.begin(), rLower.end(), rLower.begin(), ::tolower);

        // Check if query is inside Name or Roll
        if (nLower.find(qLower) != std::string::npos || rLower.find(qLower) != std::string::npos) {
            result.push_back(s);
        }
    }
    return result;
}

// 4. STATISTICS
StudentStats DataManager::getStats() {
    StudentStats stats = {0, 0.0, "N/A"};
    if (records.empty()) return stats;

    double totalCGPA = 0;
    int activeCount = 0;
    double maxCGPA = -1.0;

    for (const auto& s : records) {
        if (s.active) {
            activeCount++;
            totalCGPA += s.cgpa;
            if (s.cgpa > maxCGPA) {
                maxCGPA = s.cgpa;
                stats.topStudent = s.name + " (" + s.roll + ")";
            }
        }
    }

    if (activeCount > 0) {
        stats.totalCount = activeCount;
        stats.avgCGPA = totalCGPA / activeCount;
    }
    return stats;
}
