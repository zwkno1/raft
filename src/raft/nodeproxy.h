#pragma once

#include "message.h"

class NodeProxy
{
public:
    virtual void requestVote(NodeId node, const RequestVoteRequest & request) = 0;

    virtual void appendEntry(NodeId node, const AppendEntriesRequest & request) = 0;

    virtual void replyRequestVote(NodeId node, const RequestVoteReply & reply) = 0;

    virtual void replyAppendEntry(NodeId node, const AppendEntriesReply & reply) = 0;

    virtual ~NodeProxy() = 0;
};

typedef shared_ptr<NodeProxy> NodeProxyPtr;
