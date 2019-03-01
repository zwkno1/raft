#include <raft/nodei.h>

NodeI::NodeI(NodeProxyPtr proxy, PersistentStatePtr persistentState)
    : type_(Follower)
    , proxy_(proxy)
    , persistentState_(persistentState)
{
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

void NodeI::appendEntries(Term term, NodeId leaderId, Index prevLogIndex, Term prevLogTerm, Index leaderCommit, std::vector<LogEntry> & entries)
{
    // 1.
    if(term < persistentState_->currentTerm())
    {
        proxy_->appendEntriesReply(leaderId, persistentState_->currentTerm(), false);
        return;
    }

    if(term == persistentState_->currentTerm())
    {
        if(type_ == Leader)
        {
            // err: two leaders at same term
        }

        if(type_ == Candidate)
        {
            changeType(Follower);
            setLeaderId(leaderId_);
        }
    }
    else
    {
        updateCurrentTerm(term);
        setLeaderId(leaderId_);
    }

    persistentState_->truncateLog(prevLogTerm, prevLogIndex);
    persistentState_->appendEntries(entries);
    persistentState_->setCommitIndex(leaderCommit);

    proxy_->appendEntriesReply(leaderId, term, true);
}

/*
 * 1. Reply false if term < currentTerm (§5.1)
 * 2. If votedFor is null or candidateId, and candidate’s log is at
 * least as up-to-date as receiver’s log, grant vote (§5.2, §5.4)
 */
void NodeI::onRequestVote(const RequestVoteRequest & request)
{
    // 1.
    if(request.term < state_.currentTerm)
    {
        proxy_->replyRequestVote(request.candidateId, RequestVoteReply{state_.currentTerm, false});
        return;
    }

    if(request.term > state_.currentTerm)
    {
        // update currentTerm
        updateCurrentTerm(request.term);
    }

    // already votedFor ohter candidate
    if(state_.votedFor && (state_.votedFor.value() != request.candidateId))
    {
        proxy_->replyRequestVote(request.candidateId, RequestVoteReply{state_.currentTerm, false});
        return;
    }

    // candidate's log is not at  least as up-to-date as this log
    if(request.lastLogId < log_->lastId())
    {
        proxy_->replyRequestVote(request.candidateId, RequestVoteReply{state_.currentTerm, false});
        return;
    }

    // 2.
    voteFor(request.candidateId);

    proxy_->replyRequestVote(request.candidateId, RequestVoteReply{state_.currentTerm, true});
}

void NodeI::onAppendEntriesReply(const AppendEntriesReply &reply)
{
    if(reply.term > state_.currentTerm)
    {
        updateCurrentTerm(reply.term);
        return;
    }

    if(!reply.success)
        return;





}

void NodeI::onRequestVoteReply(const RequestVoteReply &reply)
{
    if(reply.term == state_.currentTerm)
    {
    }
    else if(reply.term < state_.currentTerm)
    {
        return;
    }
    else if(reply.term > state_.currentTerm)
    {
        updateCurrentTerm(reply.term);
    }
}

void NodeI::changeType(NodeType type)
{
    type_ = type;

    // todo:
}

