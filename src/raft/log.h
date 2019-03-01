#pragma once

#include <optional>
#include <memory>

#include "common.h"

class Log
{
public:
    virtual void appendEntries(const std::vector<LogEntry> & entries) = 0;

    virtual bool truncate(LogId id) = 0;

    virtual void dropAfter(Index idx) = 0;

    virtual ~Log() = 0;
};

typedef std::shared_ptr<Log> LogPtr;
