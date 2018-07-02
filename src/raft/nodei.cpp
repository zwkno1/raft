#include <raft/nodei.h>

NodeI::NodeI(NodeProxyPtr proxy, DatabasePtr db)
    : type_(Follower)
    , proxy_(proxy)
    , db_(db)
{
    state_ = db->loadNodeState();
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
void NodeI::onAppendEntry(const AppendEntriesRequest & request)
{
    AppendEntriesReply reply{ std::max(request.term, state_.currentTerm), false};

    if(request.term > state_.currentTerm)
    {
        NodeState state{ request.term , std::nullopt };
        db_->saveNodeState(state);
        state_ = state;
    }

    if(request.term == state_.currentTerm)
    {
        auto entry = log_->get(request.prevLogId.index);
        if(entry)
        {
            if(entry->id == request.prevLogId)
            {
                bool reachEnd = false;
                for(auto const & e : request.entries)
                {
                    if(reachEnd)
                    {
                        log_->put(e);
                    }
                    else
                    {
                        entry = log_->get(e.id.index);
                        if(!entry || (entry->id != e.id))
                        {
                            if(entry)
                            {
                                log_->dropAfter(e.id.index - 1);
                            }
                            reachEnd = true;
                            log_->put(e);
                        }
                    }
                }
                reply.success = true;
            }
            else
            {
                log_->dropAfter(request.prevLogId.index - 1);
            }
        }
    }

    commitIndex_ = std::min(request.leaderCommit, log_->lastId().index);

    proxy_->replyAppendEntry(request.leaderId, reply);
}

/*
 * 1. Reply false if term < currentTerm (§5.1)
 * 2. If votedFor is null or candidateId, and candidate’s log is at
 * least as up-to-date as receiver’s log, grant vote (§5.2, §5.4)
 */
void NodeI::onRequestVote(const RequestVoteRequest & request)
{
    RequestVoteReply reply{ std::max(request.term, state_.currentTerm), false};

    if(request.term > state_.currentTerm)
    {
        // update currentTerm
        NodeState state{ request.term , std::nullopt };
        if(request.lastLogId >= log_->lastId())
        {
            reply.voteGranted = true;
            state.votedFor = request.candidateId;
        }

        // save state
        db_->saveNodeState(state);
        state_ = state;
        changeType(Follower);
    }
    else if(request.term == state_.currentTerm)
    {
        if((!state_.votedFor) || (state_.votedFor.value() == request.candidateId))
        {
            if(request.lastLogId >= log_->lastId())
            {
                reply.voteGranted = true;
                NodeState state{ request.term, request.candidateId };
                // save state
                db_->saveNodeState(state);
                state_ = state;
            }
        }
    }

    proxy_->replyRequestVote(request.candidateId, reply);
}

void NodeI::onTick()
{
    switch (type_)
    {
    case Follower:
        break;
    case Candidate:
        break;
    case Leader:
        break;
    default:
        break;
    }

}

void NodeI::changeType(NodeType type)
{
    type_ = type;

    // todo:
}

