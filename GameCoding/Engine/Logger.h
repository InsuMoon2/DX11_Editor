#pragma once

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
};

class Logger
{
    DECLARE_SINGLE(Logger);

public:
    void Log(LogLevel level, const string& message);

    void Info(const string& message) { Log(LogLevel::Info, message); }
    void Warning(const string& message) { Log(LogLevel::Warning, message); }
    void Error(const string& message) { Log(LogLevel::Error, message); }

    void Clear() { _logs.clear(); }
    const vector<LogEntry>& GetLogs() const { return _logs; }

private:
    vector<LogEntry> _logs;
    int _maxLogs = 500; // 최대 로그 개수

};

#define LOG_INFO(msg)    GET_SINGLE(Logger)->Info(msg)
#define LOG_WARNING(msg) GET_SINGLE(Logger)->Warning(msg)
#define LOG_ERROR(msg)   GET_SINGLE(Logger)->Error(msg)
