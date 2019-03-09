#pragma once

#include <vector>

#include <raft/def.h>

/*
 * AppendEntries RPC
 *
 * invoked by leader to replicate log entries (§5.3); also used as heartbeat (§5.2).
 */

struct AppendEntriesRequest
{
    // leader's term
    Term term;

    // so follower can redirect clients new ones
    NodeId leaderId;

    // index of log entry immediately preceding new ones
    Index prevLogIndex;

    // term of prevLogIndex entry
    Term prevLogTerm;

    // log entries to store (empty for heartbeat; may send more than one for efficiency)
    std::vector<LogEntry> entries;

    // leader's commitIndex
    Index leaderCommit;
};

struct AppendEntriesReply
{
    // currentTerm, for leader to update itself
    Term term;

    // success true if follower contained entry matching prevLogIndex and prevLogTerm
    bool success;
};

/*
 * RequestVote RPC
 * Invoked by candidates to gather votes (§5.2).
 */

struct RequestVoteRequest
{
    // candidate’s term
    Term term;

    // candidate requesting vote
    NodeId candidateId;

    // index of candidate’s last log entry (§5.4)
    Index lastLogIndex;

    // term of candidate’s last log entry (§5.4)
    Term lastLogTerm;
};

struct RequestVoteReply
{
    // currentTerm, for candidate to update itself
    Term term;

    // true means candidate received vote
    bool voteGranted;
};
