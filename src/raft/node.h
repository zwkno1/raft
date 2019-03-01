#pragma once

#include <raft/message.h>

class Node
{
public:
    virtual ~Node();

    virtual NodeId id() const = 0;

    virtual NodeType type() const = 0;

    virtual void appendEntries(Term term, NodeId leaderId, Index prevLogIndex, Term prevLogTerm, Index leaderCommit, std::vector<LogEntry> & entries) = 0;

    virtual void requestVote(Term term, NodeId candidateId, Index lastLogIndex, Term lastLogTerm) = 0;

    // replies
    virtual void appendEntriesReply(Term term, bool success) = 0;

    virtual void requestVoteReply(Term term, bool voteGranted) = 0;
};
