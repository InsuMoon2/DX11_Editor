#include "pch.h"
#include "Logger.h"
#include <ctime>

#include "Utils.h"

void Logger::Log(LogLevel level, const string& message, int32_t key)
{
    lock_guard<mutex> guard(_lock);

    if(key != 0)
    {
        // 뒤에서부터 검색 (최신 로그가 뒤에 있으므로)
        for (int i = (int)_logs.size() - 1; i >= 0; i--)
        {
            // 같은 키를 가진 로그를 발견하면 -> 덮어쓰고 종료
            if (_logs[i].logKey == key)
            {
                _logs[i].level = level;
                _logs[i].message = message;
                //_logs[i].timestamp = Utils::GetCurrentTime();
                return;
            }
        }
    }


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
    entry.logKey = key;

    _logs.push_back(entry);

    // 최대 개수 초과 시 오래된 순으로 삭제
    if (_logs.size() > _maxLogs)
        _logs.erase(_logs.begin());


}

vector<LogEntry> Logger::GetLogsCopy()
{
    lock_guard<mutex> guard(_lock);

    return _logs;
}
