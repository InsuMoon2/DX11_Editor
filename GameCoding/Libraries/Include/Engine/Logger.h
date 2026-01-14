#pragma once

#include <mutex>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

struct LogEntry
{
    LogLevel level;
    string message;
    string timestamp;
    int32_t logKey = 0;
};

class Logger
{
    DECLARE_SINGLE(Logger);

public:
    void Log(LogLevel level, const string& message, int32_t key = 0);

    void Info(const string& message, int32_t key = 0) { Log(LogLevel::Info, message, key); }
    void Warning(const string& message, int32_t key = 0) { Log(LogLevel::Warning, message, key); }
    void Error(const string& message, int32_t key = 0) { Log(LogLevel::Error, message, key); }

    void Clear() { _logs.clear(); }
    const vector<LogEntry>& GetLogs() const { return _logs; }

    vector<LogEntry> GetLogsCopy();

private:
    vector<LogEntry> _logs;
    int _maxLogs = 500; // 최대 로그 개수

    mutex _lock;

};

#define LOG_INFO(...)    GET_SINGLE(Logger)->Info(__VA_ARGS__)
#define LOG_WARNING(...) GET_SINGLE(Logger)->Warning(__VA_ARGS__)
#define LOG_ERROR(...)   GET_SINGLE(Logger)->Error(__VA_ARGS__)
