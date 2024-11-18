#ifndef LOGGER_H
#define LOGGER_H

#include <string>

extern class Logger logger;

//  The Logger class handles logging strings to text files and counting character occurrences.
class Logger {
private:
    std::string directory_; // Directory where log files are stored.
    std::string current_filename_;  // Current log file name.
    int file_index_;    // Index of the current log file.
    int current_file_size_; // Size of the current log file in characters.

    void openNewFile(); // Opens a new log file.

public:
    Logger(const std::string& directory);

    void log(const std::string& input);
    void flush();
};

#endif  // LOGGER_H
