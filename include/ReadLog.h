#pragma once
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <chrono>
#include <string>
#include <thread>

class ReadLog {
public:
    enum Level {
        DEBUG = 0,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    struct message {
        std::string message;
        std::string Date;
        std::string Time;
        Level level;
    };

    void push(message mess) {
        std::unique_lock<std::mutex> lock(mtx_);
        Log_q.push(mess);
        cond_.notify_one();
    }

    void LogWork() {
        while (1) {
            message mess;
            {
                std::unique_lock<std::mutex> lock(mtx_);
                while (Log_q.empty()) {
                    cond_.wait(lock);
                }
                mess = Log_q.front();
                Log_q.pop();
            }

            std::string path = "./log/";
            path += mess.Date;
            std::filesystem::create_directories(path);
            path = path + "/" + mess.Date + ".txt";
            std::ofstream f(path, std::ios::app);
            std::string word = mess.Time + mess.message + "\n";
            if (mess.level == DEBUG) {
                word = "DEBUG:" + word;
            } else if (mess.level == WARN) {
                word = "WARN:" + word;
            } else if (mess.level == INFO) {
                word = "INFO:" + word;
            } else if (mess.level == ERROR) {
                word = "ERROR:" + word;
            } else if (mess.level == FATAL) {
                word = "FATAL:" + word;
            }
            f << word;
        }
    }

    static ReadLog& instance();
    static std::string getCurrentDate();
    static std::string getCurrentTime();

private:
    ReadLog() {
        std::thread(&ReadLog::LogWork, this).detach();
    }
    std::condition_variable cond_;
    std::mutex mtx_;
    std::queue<message> Log_q;
};

inline ReadLog& ReadLog::instance() {
    static ReadLog readLog;
    return readLog;
}

inline std::string ReadLog::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);
    auto tm = std::localtime(&currentTime);
    std::string currentDate = std::to_string(tm->tm_year + 1900) + '-'
                            + std::to_string(tm->tm_mon + 1) + '-'
                            + std::to_string(tm->tm_mday);
    return currentDate;
}

inline std::string ReadLog::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);
    auto tm = std::localtime(&currentTime);
    std::string currentDate = "[" + std::to_string(tm->tm_year + 1900) + '-'
                            + std::to_string(tm->tm_mon + 1) + '-'
                            + std::to_string(tm->tm_mday) + ' '
                            + std::to_string(tm->tm_hour) + ':'
                            + std::to_string(tm->tm_min) + ':'
                            + std::to_string(tm->tm_sec) + "] ";
    return currentDate;
}

// 快捷宏：自动填充时间和日期
#define LOG_DEBUG(msg) do { \
    ReadLog::message _m; \
    _m.message = (msg); \
    _m.Date = ReadLog::getCurrentDate(); \
    _m.Time = ReadLog::getCurrentTime(); \
    _m.level = ReadLog::DEBUG; \
    ReadLog::instance().push(_m); \
} while(0)

#define LOG_INFO(msg) do { \
    ReadLog::message _m; \
    _m.message = (msg); \
    _m.Date = ReadLog::getCurrentDate(); \
    _m.Time = ReadLog::getCurrentTime(); \
    _m.level = ReadLog::INFO; \
    ReadLog::instance().push(_m); \
} while(0)

#define LOG_WARN(msg) do { \
    ReadLog::message _m; \
    _m.message = (msg); \
    _m.Date = ReadLog::getCurrentDate(); \
    _m.Time = ReadLog::getCurrentTime(); \
    _m.level = ReadLog::WARN; \
    ReadLog::instance().push(_m); \
} while(0)

#define LOG_ERROR(msg) do { \
    ReadLog::message _m; \
    _m.message = (msg); \
    _m.Date = ReadLog::getCurrentDate(); \
    _m.Time = ReadLog::getCurrentTime(); \
    _m.level = ReadLog::ERROR; \
    ReadLog::instance().push(_m); \
} while(0)

#define LOG_FATAL(msg)do{\
ReadLog::message _m;\
_m.message=(msg);\
_m.Date=ReadLog::getCurrentDate();\
_m.Time=ReadLog::getCurrentTime();\
_m.level=ReadLog::FATAL;\
ReadLog::instance().push(_m); \
std::this_thread::sleep_for(std::chrono::milliseconds(100));\
exit(EXIT_FAILURE);\
}while(0)
