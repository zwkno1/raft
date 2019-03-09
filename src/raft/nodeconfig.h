#pragma once
#include <raft/def.h>

struct NodeConfig
{
    // election timeout range (ms)
    std::pair<Duration, Duration> electionTimeout;

    // heartbeat timeout (ms)
    Duration heartbeatTimeout;

    // leader send heartbeat < heartbeatTimeout + network delay (ms)
    Duration leaderHeartbeatGap;

    // max log entry num in appendEntries
    std::size_t maxLogEntryNum;
};
