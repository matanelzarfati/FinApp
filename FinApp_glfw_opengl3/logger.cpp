#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <queue>
#include <algorithm>
#include <ctime>
#include "logger.h"

const bool CNST_OUTPUT_MUTEX_STATUS_LOGGER = false;
const bool CNST_OUTPUT_MUTEX_STATUS_COUT = false;


Logger logger("Logs_FinApp");

std::mutex logger_mutex;
std::queue<std::string> logger_queue;

//  The constructor initializes the logging directory. If the directory exists,
//  it clears all files within it. If the directory does not exist, it attempts to create it.
Logger::Logger(const std::string& directory) : directory_(directory), file_index_(0), current_file_size_(0) {
    if (std::filesystem::exists(directory)) {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!std::filesystem::remove(entry.path())) {
                std::cout << "Can’t prepare the directory." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        if (!std::filesystem::create_directory(directory)) {
            std::cout << "Can’t prepare the directory." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    openNewFile();
}


//  This method updates the current filename to a new file with the format "inputN.txt",
//  where N is the current file index. It also resets the current file size to 0.
void Logger::openNewFile() {
    current_filename_ = directory_ + "/input" + std::to_string(file_index_) + ".txt";
    file_index_++;
    current_file_size_ = 0;
}


// thread safe
void Logger::log(const std::string& input) {
    logger_mutex.lock();
    if(CNST_OUTPUT_MUTEX_STATUS_COUT) std::cout << "log(): Mutex locked" << std::endl;
    if(CNST_OUTPUT_MUTEX_STATUS_LOGGER) logger_queue.push("log(): Mutex locked");
    logger_queue.push(input);
    if (CNST_OUTPUT_MUTEX_STATUS_LOGGER) logger_queue.push("log(): Mutex unlocked");
    if (CNST_OUTPUT_MUTEX_STATUS_COUT) std::cout << "log(): Mutex unlocked" << std::endl;
    logger_mutex.unlock();
}


//  This method appends the input string to the current log file. If adding the string would
//  exceed 100 characters in the current file, a new file is opened.
//  Each logged string is followed by a newline character.
void Logger::flush() {

    logger_mutex.lock();
    if (CNST_OUTPUT_MUTEX_STATUS_COUT) std::cout << "log(): Mutex locked" << std::endl;
    if (CNST_OUTPUT_MUTEX_STATUS_LOGGER) logger_queue.push("log(): Mutex locked");
    if (logger_queue.empty()) {
        if (CNST_OUTPUT_MUTEX_STATUS_LOGGER) logger_queue.push("log(): Mutex unlocked");
        if (CNST_OUTPUT_MUTEX_STATUS_COUT) std::cout << "log(): Mutex unlocked" << std::endl;
        logger_mutex.unlock();
        return;
    }
    std::string input = logger_queue.front();
    logger_queue.pop(); // remove
    if (CNST_OUTPUT_MUTEX_STATUS_LOGGER) logger_queue.push("log(): Mutex unlocked");
    if (CNST_OUTPUT_MUTEX_STATUS_COUT) std::cout << "log(): Mutex unlocked" << std::endl;
    logger_mutex.unlock();

    // Get current time as time_t object
    std::time_t currentTime = std::time(nullptr);

    // Create tm struct to hold local time
    std::tm localTime;

    // Convert time_t to tm struct for local time safely
    localtime_s(&localTime, &currentTime);

    std::string temp_input = "- (" + std::to_string(localTime.tm_mday) + "/" 
        + std::to_string(localTime.tm_mon + 1) + "/" + std::to_string(localTime.tm_year + 1900) 
        + " " + std::to_string(localTime.tm_hour) + ":"
        + std::to_string(localTime.tm_min) + ":" + std::to_string(localTime.tm_sec) + ") - " + input;

    if (current_file_size_ + temp_input.size() > 1500)
        openNewFile();

    std::ofstream file(current_filename_, std::ios::app);   // equiv. to open the file
    if (!file.is_open()) {
        std::cout << "Can’t open the file " << std::filesystem::path(current_filename_).filename().string()
            << std::endl;
        exit(EXIT_FAILURE);
    }

    file << temp_input << '\n';
    current_file_size_ += temp_input.size(); // Summed up the file size.
    file.close();
}
