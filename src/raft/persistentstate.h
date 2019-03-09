#pragma once

#include "def.h"

#include <set>
#include <optional>

class PersistentState
{
public:
    virtual ~PersistentState();

    // this node id
    virtual NodeId nodeId() = 0;

    // get all nodes include this node
    virtual std::set<NodeId> allNodes() = 0;

    virtual Term currentTerm() = 0;

	virtual Index commitIndex() = 0;

    // return votedFor in current term, return 0 if none
    virtual NodeId votedFor() = 0;

    // return last log entry
	virtual LogEntry lastLogEntry() = 0;

    // return term of log at index, reutrn 0 if not found
    virtual Term getLogTerm(Index index) = 0;

    // check if log (index, term) uptodate than last log
	virtual bool isUpToDate(Index index, Term term) = 0;

    // update current term and clear votedFor
    virtual bool updateCurrentTerm(Term term) = 0;

    // update votedFor
    virtual bool updateVotedFor(NodeId id) = 0;

    // update commit index
	virtual bool updateCommitIndex(Index index) = 0;

	virtual bool appendEntries(const std::vector<LogEntry> & entries) = 0;

};

PersistentState::~PersistentState()
{
}

typedef std::shared_ptr<PersistentState> PersistentStatePtr;

