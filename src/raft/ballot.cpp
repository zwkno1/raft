#include <raft/ballot.h>

Ballot::Ballot()
    : acceptNum_(0)
    , rejectNum_(0)
{
}

void Ballot::reset(const std::set<NodeId> & nodes)
{
    acceptNum_ = 0;
    rejectNum_ = 0;

    nodeResults_.clear();
    for(auto i : nodes)
    {
        nodeResults_[i] = BALLOT_UNKNOWN;
    }
}

void Ballot::accept(NodeId id)
{
    auto iter = nodeResults_.find(id);
    if(iter == nodeResults_.end())
        return;
    if(iter->second == BALLOT_UNKNOWN)
    {
        iter->second = BALLOT_ACCEPT;
        ++acceptNum_;
    }
}

void Ballot::reject(NodeId id)
{
    auto iter = nodeResults_.find(id);
    if(iter == nodeResults_.end())
        return;
    if(iter->second == BALLOT_UNKNOWN)
    {
        iter->second = BALLOT_REJECT;
        ++rejectNum_;
    }
}


BallotResult Ballot::nodeResult(NodeId id) const
{
    auto iter = nodeResults_.find(id);
    if(iter == nodeResults_.end())
        return BALLOT_UNKNOWN;
    return iter->second;
}

BallotResult Ballot::result() const
{
    if(acceptNum_ > nodeResults_.size() / 2)
    {
        return  BALLOT_ACCEPT;
    }

    if(rejectNum_ > nodeResults_.size() / 2)
    {
        return  BALLOT_ACCEPT;
    }

    return BALLOT_UNKNOWN;
}

