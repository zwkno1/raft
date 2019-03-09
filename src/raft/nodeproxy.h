#pragma once

#include <raft/message.h>

class NodeProxy
{
public:
    virtual ~NodeProxy();

    virtual void appendEntries(NodeId id, AppendEntriesRequest & request) = 0;

    virtual void requestVote(NodeId id, RequestVoteRequest & request) = 0;
};

NodeProxy::~NodeProxy()
{
}

typedef std::shared_ptr<NodeProxy> NodeProxyPtr;
