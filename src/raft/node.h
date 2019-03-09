#pragma once

#include <raft/message.h>

class Node
{
public:
    virtual ~Node();

    virtual NodeId id() const = 0;

    virtual NodeType type() const = 0;

    virtual AppendEntriesReply onAppendEntries(AppendEntriesRequest & request) = 0;

    virtual RequestVoteReply onRequestVote(RequestVoteRequest & request) = 0;

    virtual void onAppendEntriesReply(NodeId nodeId, Index beginIndex, Index endIndex, AppendEntriesReply & reply) = 0;

    virtual void onRequestVoteReply(NodeId nodeId, RequestVoteReply & reply) = 0;

    virtual void onTick(TimePoint now) = 0;
};

Node::~Node()
{
}
