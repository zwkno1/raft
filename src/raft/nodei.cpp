#include <raft/nodei.h>

#include <algorithm>
#include <chrono>
#include <raft/ballot.h>

NodeI::NodeI(PersistentStatePtr persistentState, NodeProxyPtr nodeProxy)
    : persistentState_(persistentState)
    , nodeProxy_(nodeProxy)
    , type_(Follower)
    , random_(std::random_device{}())
{
    changeType(Follower);
}

NodeId NodeI::id() const
{
    return persistentState_->nodeId();
}

NodeType NodeI::type() const
{
    return type_;
}

/*
 * 1. Reply false if term < currentTerm (§5.1)
 * 2. Reply false if log doesn’t contain an entry at prevLogIndex
 * whose term matches prevLogTerm (§5.3)
 * 3. If an existing entry conflicts with a new one (same index
 * but different terms), delete the existing entry and all that
 * follow it (§5.3)
 * 4. Append any new entries not already in the log
 * 5. If leaderCommit > commitIndex, set commitIndex =
 * min(leaderCommit, index of last new entry)
*/
AppendEntriesReply NodeI::onAppendEntries(AppendEntriesRequest & request)
{
    // 1.
    if(request.term < persistentState_->currentTerm())
    {
        return AppendEntriesReply{ persistentState_->currentTerm(), false};
    }

    // update currentTerm
    if(request.term < persistentState_->currentTerm())
    {
        persistentState_->updateCurrentTerm(request.term);
    }

    if(type_ != Follower)
    {
        changeType(Follower);
        setLeaderId(request.leaderId);
    }

    // 2. 3. 4.
    if(!persistentState_->appendEntries(request.entries))
    {
        return AppendEntriesReply{ persistentState_->currentTerm(), false};
    }

    // 5.
    if(request.leaderCommit > persistentState_->commitIndex())
    {
        persistentState_->updateCommitIndex(std::min(request.leaderCommit, persistentState_->lastLogEntry().index));
    }

    return AppendEntriesReply{ persistentState_->currentTerm(), true};
}

/*
 * 1. Reply false if term < currentTerm (§5.1)
 * 2. If votedFor is null or candidateId, and candidate’s log is at
 * least as up-to-date as receiver’s log, grant vote (§5.2, §5.4)
 */
RequestVoteReply NodeI::onRequestVote(RequestVoteRequest & request)
{
    // 1.
    if(request.term < persistentState_->currentTerm())
    {
        return RequestVoteReply{ persistentState_->currentTerm(), false};
    }

    // update currentTerm
    if(request.term > persistentState_->currentTerm())
    {
        // do not update if leader is living
        if(volatileState_.activeTime + config_.heartbeatTimeout > Clock::now())
        {
            persistentState_->updateCurrentTerm(request.term);
        }
    }

    // 2.
    if(!persistentState_->isUpToDate(request.lastLogIndex, request.lastLogTerm))
    {
        return RequestVoteReply{ persistentState_->currentTerm(), false};
    }

    if(!persistentState_->updateVotedFor(request.candidateId))
    {
        return RequestVoteReply{ persistentState_->currentTerm(), false};
    }

    // garanted vote, change to follower

    changeType(Follower);

    return RequestVoteReply{ persistentState_->currentTerm(), true};
}

void NodeI::onAppendEntriesReply(NodeId nodeId, Index beginIndex, Index endIndex, AppendEntriesReply & reply)
{
    if(reply.term > persistentState_->currentTerm())
    {
        persistentState_->updateCurrentTerm(reply.term);
        changeType(Follower);
        return;
    }

    // ignore
    if(type() != Leader)
        return;

    // refresh activeTime of nodeId
    volatileState_.followerState[nodeId].activeTime = Clock::now();

    // heartbeat reply
    if(beginIndex == endIndex)
        return;

    if(reply.term == persistentState_->currentTerm())
    {
        if(reply.success)
        {
            FollowerState & followerState = volatileState_.followerState[nodeId];
            followerState.matchIndex = endIndex - 1;
            followerState.nextIndex = endIndex;

            // update commitIndex if log replicate to majority of the node at currentTerm
            if((followerState.matchIndex > persistentState_->commitIndex()) && (persistentState_->getLogTerm(followerState.matchIndex) == persistentState_->currentTerm()))
            {
                std::vector<Index> indexs;
                indexs.reserve(volatileState_.followerState.size());
                for(auto & i : volatileState_.followerState)
                {
                    indexs.push_back(i.second.matchIndex);
                }
                std::sort(indexs.begin(), indexs.end());
                Index commitIndex = indexs[indexs.size()/2+1];
                if(commitIndex > persistentState_->commitIndex())
                {
                    persistentState_->updateCommitIndex(commitIndex);
                    // todo: apply logs to statemachine
                }
            }
        }
        else
        {
            // decrease nextIndex of reply.followerId until success
            volatileState_.followerState[nodeId].nextIndex = beginIndex - config_.maxLogEntryNum;
        }
    }

}

void NodeI::onRequestVoteReply(NodeId nodeId, RequestVoteReply & reply)
{
    if(reply.term > persistentState_->currentTerm())
    {
        persistentState_->updateCurrentTerm(reply.term);
        changeType(Follower);
        return;
    }

    if(reply.term == persistentState_->currentTerm())
    {
        if(type_ != Candidate)
            return;

        if(ballot_.result() != BALLOT_UNKNOWN)
            return;

        if(reply.voteGranted)
            ballot_.accept(nodeId);
        else
            ballot_.reject(nodeId);

        switch(ballot_.result())
        {
        case BALLOT_ACCEPT:
            changeType(Leader);
            break;
        case BALLOT_REJECT:
            changeType(Follower);
            break;
        default:
            break;
        }
    }
}

void NodeI::onTick(TimePoint now)
{
    switch (type())
    {
    case Leader:
    {
        for(auto & i : volatileState_.followerState)
        {
            // send heartbeat
            if(i.second.activeTime + config_.leaderHeartbeatGap >= now)
            {
                AppendEntriesRequest request;
                request.term = persistentState_->currentTerm();
                request.leaderId = id();
                request.prevLogIndex = i.second.matchIndex;
                request.prevLogTerm = persistentState_->getLogTerm(i.second.matchIndex);
                request.leaderCommit = persistentState_->commitIndex();
                nodeProxy_->appendEntries(i.first, request);
            }
        }

    }
        break;
    case Follower:
    {
        if(volatileState_.activeTime + volatileState_.electionTimeout >= now)
        {
            changeType(Candidate);
        }
    }
        break;
    case Candidate:
    {
        // election timeout, start new election
        if(volatileState_.activeTime + volatileState_.electionTimeout >= now)
        {
            changeType(Candidate);
            return;
        }

        for(auto & i : volatileState_.followerState)
        {
            if(ballot_.nodeResult(i.first) != BALLOT_UNKNOWN)
                continue;

            // RequestVote timeout, retry
            if(i.second.activeTime + config_.heartbeatTimeout >= now)
            {
                RequestVoteRequest request;
                request.term = persistentState_->currentTerm();
                request.candidateId = id();
                request.lastLogIndex = persistentState_->lastLogEntry().index;
                request.lastLogTerm = persistentState_->lastLogEntry().term;
                nodeProxy_->requestVote(i.first, request);
            }
        }
    }
        break;
    }
}

void NodeI::changeType(NodeType type)
{
    type_ = type;

    // initial for each type

    switch (type_)
    {
    case Leader:
    {
        for(auto & i : volatileState_.followerState)
        {
            i.second.matchIndex = 0;
            i.second.nextIndex = persistentState_->lastLogEntry().index + 1;
            i.second.activeTime = Clock::now();
        }
    }
        break;
    case Follower:
    {
        volatileState_.electionTimeout = distribution_(random_) * 1ms;
        volatileState_.commitIndex = 0;
        volatileState_.lastApplied = 0;
    }
        break;
    case Candidate:
    {
        // randomize election timeout
        volatileState_.electionTimeout = distribution_(random_) * 1ms;

        // increase term
        persistentState_->updateCurrentTerm(persistentState_->currentTerm() + 1);

        // init ballot
        ballot_.reset(persistentState_->allNodes());

        // vote for self
        persistentState_->updateVotedFor(id());
        ballot_.accept(id());

        // send RequestVote to all nodes
        for(auto & i : volatileState_.followerState)
        {
            // RequestVote
            RequestVoteRequest request;
            request.term = persistentState_->currentTerm();
            request.candidateId = id();
            request.lastLogIndex = persistentState_->lastLogEntry().index;
            request.lastLogTerm = persistentState_->lastLogEntry().term;
            nodeProxy_->requestVote(i.first, request);

            // retry RequestVote if timeout
            i.second.activeTime = Clock::now();
        }
    }
        break;
    }
}
