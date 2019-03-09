#pragma once

#include <unordered_map>
#include <set>

#include <raft/def.h>

// todo: support membership chage

enum BallotResult
{
    BALLOT_UNKNOWN = 0,
    BALLOT_ACCEPT,
    BALLOT_REJECT,
};

class Ballot
{
public:
    Ballot();

    void reset(const std::set<NodeId> & node);

    void accept(NodeId id);

    void reject(NodeId id);

    BallotResult nodeResult(NodeId id) const;

    BallotResult result() const;

private:
    std::unordered_map<NodeId, BallotResult> nodeResults_;

    std::size_t acceptNum_;

    std::size_t rejectNum_;
};
