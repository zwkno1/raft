#pragma once

#include <unordered_map>

#include <raft/def.h>

struct FollowerState
{
    /*
     * for each server, index of the next log entry
     * to send to that server (initialized to leader
     * last log index + 1)
     */
    Index nextIndex;

    /*
     * for each server, index of highest log entry
     * known to be replicated on server
     * (initialized to 0, increases monotonically)
     */
    Index matchIndex;

    // last appendEntries return time of this follower
    TimePoint activeTime;

};

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
    std::unordered_map<NodeId, FollowerState> followerState;

    // Volatile state on followers:
    // last appendEntries time
    TimePoint activeTime;

    // election timeout (ms)
    Duration electionTimeout;
};

