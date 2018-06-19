#pragma once

#include <vector>

#include "state.h"

/*
 * AppendEntries RPC
 *
 * Invoked by leader to replicate log entries (§5.3); also used as
 * heartbeat (§5.2).
 *
 * Receiver implementation:
 * 1. Reply false if term < currentTerm (§5.1)
 * 2. Reply false if log doesn’t contain an entry at prevLogIndex
 * whose term matches prevLogTerm (§5.3)
 * 3. If an existing entry conflicts with a new one (same index
 * but different terms), delete the existing entry and all that
 * follow it (§5.3)
 * 4. Append any new entries not already in the log
 * 5. If leaderCommit > commitIndex, set commitIndex =
 * min(leaderCommit, index of last new entry)
*/

class AppendEntriesRequest
{
    /*
     * leader’s term
     */
    Term term;

    /*
     * so follower can redirect client
     */
    ServerId leaderId;

    /*
     * index of log entry immediately preceding
     * new ones
     */
    Term prevLogIndex;

    /*
     * term of prevLogIndex entry
     */
    Term prevLogTerm;

    /*
     * log entries to store (empty for heartbeat;
     * may send more than one for efficiency)
     */
    std::vector<Value> entries;

    /*
     * leader’s commitIndex
     */
    Index leaderCommit;
};

class AppendEntriesReply
{
    /*
     * currentTerm, for leader to update itself
     */
    Term term;

    /*
     * true if follower contained entry matching
     * prevLogIndex and prevLogTerm
     */
    bool success;
};


/* RequestVote RPC
 * Invoked by candidates to gather votes (§5.2).
 *
 * Receiver implementation:
 * 1. Reply false if term < currentTerm (§5.1)
 * 2. If votedFor is null or candidateId, and candidate’s log is at
 * least as up-to-date as receiver’s log, grant vote (§5.2, §5.4)
 */

class RequestVoteRequest
{
    // candidate’s term
    Term term;

    //candidate requesting vote
    ServerId candidateId;

    // index of candidate’s last log entry (§5.4)
    Index lastLogIndex;

    // term of candidate’s last log entry (§5.4)
    Term lastLogTerm;
};

class RequestVoteReply
{
    // currentTerm, for candidate to update itself
    Term term;

    // true means candidate received vote
    bool voteGranted;
};
