#pragma once

#include <optional>
#include <map>

#include <raft/node.h>
#include <raft/log.h>
#include <raft/persistentstate.h>
#include <raft/nodeproxy.h>
#include <raft/volatilestate.h>

class NodeI : public Node
{
public:
    NodeI(NodeProxyPtr proxy, PersistentStatePtr persistentState);

    NodeId id() const override;

    NodeType type() const override;

    void appendEntries(Term term, NodeId leaderId, Index prevLogIndex, Term prevLogTerm, Index leaderCommit, std::vector<LogEntry> & entries) override;

    virtual void requestVote(Term term, NodeId candidateId, Index lastLogIndex, Term lastLogTerm) override;

    // replies
    void appendEntriesReply(Term term, bool success) override;

    void requestVoteReply(Term term, bool voteGranted) override;
private:
    void changeType(NodeType type);

    void setLeaderId(NodeId leaderId);

    void updateCurrentTerm(Term term);

    void voteFor(NodeId id);

    VolatileState state_;

    NodeType type_;

    NodeId leaderId_;

    NodeProxyPtr proxy_;

    PersistentStatePtr persistentState_;
};

