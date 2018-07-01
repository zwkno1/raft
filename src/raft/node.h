#pragma once

#include <raft/message.h>

class Node
{
public:
    virtual void onAppendEntry(const AppendEntriesRequest & request) = 0;

    virtual void onRequestVote(const RequestVoteRequest & request) = 0;

    virtual void onTick() = 0;

    virtual ~Node(){ }
};
