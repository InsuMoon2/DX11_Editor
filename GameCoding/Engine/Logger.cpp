#include "pch.h"
#include "Logger.h"
#include <ctime>

void Logger::Log(LogLevel level, const string& message)
{
    // 타임 스탬프
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    char timestamp[32];
    sprintf_s(timestamp, "[%02d:%02d:%02d]", ltm.tm_hour, ltm.tm_min, ltm.tm_sec);

    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.timestamp = timestamp;

    _logs.push_back(entry);

    // 최대 개수 초과 시 오래된 순으로 삭제
    if (_logs.size() > _maxLogs)
        _logs.erase(_logs.begin());


}
