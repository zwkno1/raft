#pragma once

#include <optional>
#include <memory>

#include "common.h"

class LogManager
{
public:
    virtual void put(const LogEntry & entry) = 0;

    virtual std::optional<LogEntry> get(Index idx) = 0;

    virtual void drop(Index idx) = 0;

    virtual void dropAfter(Index idx) = 0;

    virtual LogId lastId() = 0;

    virtual ~LogManager() = 0;
};

typedef std::shared_ptr<LogManager> LogManagerPtr;
