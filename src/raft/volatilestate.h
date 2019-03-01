#pragma once

#include <raft/log.h>
#include <map>

struct VolatileState
{
    // Volatile state on all servers:

    /*
     * index of highest log entry known to be
     * committed (initialized to 0, increases
     * monotonically)
     */
    Index commitIndex;

    /*
     * index of highest log entry applied to state
     * machine (initialized to 0, increases
     * monotonically)
     */
    Index lastApplied;

    // Volatile state on leaders:
    // (Reinitialized after election)

    /*
     * for each server, index of the next log entry
     * to send to that server (initialized to leader
     * last log index + 1)
     */
    std::map<NodeId, Index> nextIndex;

    /*
     * for each server, index of highest log entry
     * known to be replicated on server
     * (initialized to 0, increases monotonically)
     */
    std::map<NodeId, Index> matchIndex;
};

