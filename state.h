#pragma once

#include <optional>
#include <vector>

#include "def.h"

class CommonState
{
    // persistent

    /*
     * latest term server has seen (initialized to 0
     * on first boot, increases monotonically)
     */
    Term currentTerm_;

    /*
     * candidateId that received vote in current
     * term (or null if none)
     */
    std::optional<ServerId> votedFor_;

    // volatile

    /*
     * index of highest log entry known to be
     * committed (initialized to 0, increases
     * monotonically)
     */
    Index commitIndex_;

    /*
     * index of highest log entry applied to state
     * machine (initialized to 0, increases
     * monotonically)
     */
    Index lastApplied_;

    /*
     * log[]
     * log entries; each entry contains command
     * for state machine, and term when entry
     * was received by leader (first index is 1)
     */

};

struct LeaderState
{
    // volatile
    /*
     * for each server, index of the next log entry
     * to send to that server (initialized to leader
     * last log index + 1)
     */
    std::vector<Index> nextIndex_;

    /*
     * for each server, index of highest log entry
     * known to be replicated on server
     * (initialized to 0, increases monotonically)
     */
    std::vector<Index> matchIndex_;
};
