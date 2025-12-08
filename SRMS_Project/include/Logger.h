// include/Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
public:
    // create logger that appends to file (default "activity.log")
    explicit Logger(const std::string &path = "data/activity.log");

    // append one line (timestamp not included here)
    void append(const std::string &msg);

private:
    std::string logfile;
};

#endif // LOGGER_H
