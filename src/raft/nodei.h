#pragma once

#include <optional>

#include <raft/node.h>
#include <raft/persistentstate.h>
#include <raft/volatilestate.h>
#include <raft/ballot.h>
#include <raft/nodeconfig.h>
#include <raft/nodeproxy.h>
#include <chrono>

// implement of Node

class NodeI : public Node
{
public:
    NodeI(PersistentStatePtr persistentState, NodeProxyPtr nodeProxy);

    NodeId id() const override;

    NodeType type() const override;

    AppendEntriesReply onAppendEntries(AppendEntriesRequest & request) override;

    RequestVoteReply onRequestVote(RequestVoteRequest & request) override;

    void onAppendEntriesReply(NodeId nodeId, Index beginIndex, Index endIndex, AppendEntriesReply & reply) override;

    void onRequestVoteReply(NodeId nodeId, RequestVoteReply & reply) override;

    void onTick(TimePoint now) override;

private:
    void setLeaderId(NodeId leaderId)
    {
        leaderId_ = leaderId;
    }

    void changeType(NodeType type);

    PersistentStatePtr persistentState_;

    VolatileState volatileState_;

    NodeProxyPtr nodeProxy_;

    NodeType type_;

    NodeId leaderId_;

    Ballot ballot_;

    NodeConfig config_;

    std::mt19937 random_;

    std::uniform_int_distribution<uint32_t> distribution_;

};

