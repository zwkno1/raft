#pragma once

#include <optional>
#include <map>

#include <raft/node.h>
#include <raft/log.h>
#include <raft/database.h>
#include <raft/nodeproxy.h>

class NodeI : public Node
{
public:
    NodeI(NodeProxyPtr proxy, DatabasePtr db);

    void onAppendEntry(const AppendEntriesRequest & request) override;

    void onRequestVote(const RequestVoteRequest & request) override;

    void onTick() override;

private:
    void changeType(NodeType type);

    // persistent state
    NodeState state_;

    // volatile

    /*
     * index of highest log entry known to be
     * committed (initialized to 0, increases
     * monotonically)
     */
    Index commitIndex_;

    /*
     * index of highest log entry applied to state
     * machine (initialized to 0, increases
     * monotonically)
     */
    Index lastApplied_;

    /*
     * log entries; each entry contains command
     * for state machine, and term when entry
     * was received by leader (first index is 1)
     */

    LogManagerPtr log_;

    // leader state

    // volatile

    /*
     * for each server, index of the next log entry
     * to send to that server (initialized to leader
     * last log index + 1)
     */
    std::map<NodeId, Index> nextIndex_;

    /*
     * for each server, index of highest log entry
     * known to be replicated on server
     * (initialized to 0, increases monotonically)
     */
    std::map<NodeId, Index> matchIndex_;

    NodeType type_;

    NodeProxyPtr proxy_;

    DatabasePtr db_;
};

