#pragma once

#include "common.h"

class PersistentState
{
public:
    virtual ~PersistentState();

    virtual Term currentTerm() = 0;

    virtual std::optional<NodeId> votedFor() = 0;

    virtual void save(Term term, std::optional<NodeId> votedFor) = 0;

    virtual void appendEntries(const std::vector<LogEntry> & entries) = 0;
    virtual void truncateLog(Term term, Index index) = 0;
    virtual void setCommitIndex(Index index) = 0;

    // todo: get logEntry

};
typedef std::shared_ptr<PersistentState> PersistentStatePtr;

