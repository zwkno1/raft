#pragma once

#include "common.h"

class NodeProxy
{
public:
    virtual void appendEntries(NodeId id, Term term, NodeId leaderId, Index prevLogIndex, Term prevLogTerm, Index leaderCommit, std::vector<LogEntry> & entries) = 0;

    virtual void requestVote(NodeId id, Term term, NodeId candidateId, Index lastLogIndex, Term lastLogTerm) = 0;

    // replies
    virtual void appendEntriesReply(NodeId id, Term term, bool success) = 0;

    virtual void requestVoteReply(NodeId id, Term term, bool voteGranted) = 0;

    virtual ~NodeProxy() = 0;
};

typedef std::shared_ptr<NodeProxy> NodeProxyPtr;
