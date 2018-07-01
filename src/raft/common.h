#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <memory>

typedef std::uint64_t Term;
typedef std::uint64_t NodeId;
typedef std::uint64_t Index;
typedef std::string Value;

template<class T>
using shared_ptr = std::shared_ptr<T>;

/*
 * Rules for Servers
 *
 * All Servers:
 * • If commitIndex > lastApplied: increment lastApplied, apply
 * log[lastApplied] to state machine (§5.3)
 * • If RPC request or response contains term T > currentTerm:
 * set currentTerm = T, convert to follower (§5.1)
 *
 * Followers (§5.2):
 * • Respond to RPCs from candidates and leaders
 * • If election timeout elapses without receiving AppendEntries
 * RPC from current leader or granting vote to candidate:
 * convert to candidate
 *
 * Candidates (§5.2):
 * • On conversion to candidate, start election:
 * • Increment currentTerm
 * • Vote for self
 * • Reset election timer
 * • Send RequestVote RPCs to all other servers
 * • If votes received from majority of servers: become leader
 * • If AppendEntries RPC received from new leader: convert to
 * follower
 * • If election timeout elapses: start new election
 *
 * Leaders:
 * • Upon election: send initial empty AppendEntries RPCs
 * (heartbeat) to each server; repeat during idle periods to
 * prevent election timeouts (§5.2)
 * • If command received from client: append entry to local log,
 * respond after entry applied to state machine (§5.3)
 * • If last log index ≥ nextIndex for a follower: send
 * AppendEntries RPC with log entries starting at nextIndex
 * • If successful: update nextIndex and matchIndex for
 * follower (§5.3)
 * • If AppendEntries fails because of log inconsistency:
 * decrement nextIndex and retry (§5.3)
 * • If there exists an N such that N > commitIndex, a majority
 * of matchIndex[i] ≥ N, and log[N].term
*/

enum NodeType
{
    Follower = 0,
    Candidate,
    Leader,
};


class LogId
{
public:
    bool operator == (const LogId & other) const
    {
        return (index == other.index) && (term == other.term);
    }

    bool operator != (const LogId & other) const
    {
        return !(*this == other);
    }

    bool operator < (const LogId & other) const
    {
        if(term < other.term)
        {
            return true;
        }
        else if(term == other.term)
        {
            if(index < other.index)
                return true;
        }

        return false;
    }

    bool operator <= (const LogId & other) const
    {
        return (*this == other) || (*this < other);
    }

    bool operator > (const LogId & other) const
    {
        return !(*this <= other);
    }

    bool operator >= (const LogId & other) const
    {
        return !(*this < other);
    }

    Index index;

    Term term;
};

struct LogEntry
{
    LogId id;

    Value value;
};

struct NodeState
{
    /*
     * latest term server has seen (initialized to 0
     * on first boot, increases monotonically)
     */
    Term currentTerm;

    /*
     * candidateId that received vote in current
     * term (or null if none)
     */
    std::optional<NodeId> votedFor;
};
