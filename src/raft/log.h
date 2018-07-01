#pragma once

#include <optional>
#include <memory>

#include "common.h"

class LogManager
{
public:
    virtual void putEntry(const LogEntry & entry) = 0;

    virtual std::optional<LogEntry> getEntry(Index idx) = 0;

    virtual void dropEntry(Index idx) = 0;

    virtual void dropEntriesAfter(Index idx) = 0;

    virtual LogId lastId() = 0;
};

typedef std::shared_ptr<LogManager> LogManagerPtr;
