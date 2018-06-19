#pragma once

#include <cstdint>
#include <string>

typedef std::uint64_t Term;
typedef std::uint64_t ServerId;
typedef std::uint64_t Index;
typedef std::string Value;

/*
 * Rules for Servers
 *
 * All Servers:
 * • If commitIndex > lastApplied: increment lastApplied, apply
 * log[lastApplied] to state machine (§5.3)
 * • If RPC request or response contains term T > currentTerm:
 * set currentTerm = T, convert to follower (§5.1)
 *
 * Followers (§5.2):
 * • Respond to RPCs from candidates and leaders
 * • If election timeout elapses without receiving AppendEntries
 * RPC from current leader or granting vote to candidate:
 * convert to candidate
 *
 * Candidates (§5.2):
 * • On conversion to candidate, start election:
 * • Increment currentTerm
 * • Vote for self
 * • Reset election timer
 * • Send RequestVote RPCs to all other servers
 * • If votes received from majority of servers: become leader
 * • If AppendEntries RPC received from new leader: convert to
 * follower
 * • If election timeout elapses: start new election
 *
 * Leaders:
 * • Upon election: send initial empty AppendEntries RPCs
 * (heartbeat) to each server; repeat during idle periods to
 * prevent election timeouts (§5.2)
 * • If command received from client: append entry to local log,
 * respond after entry applied to state machine (§5.3)
 * • If last log index ≥ nextIndex for a follower: send
 * AppendEntries RPC with log entries starting at nextIndex
 * • If successful: update nextIndex and matchIndex for
 * follower (§5.3)
 * • If AppendEntries fails because of log inconsistency:
 * decrement nextIndex and retry (§5.3)
 * • If there exists an N such that N > commitIndex, a majority
 * of matchIndex[i] ≥ N, and log[N].term
*/

enum Role
{
    Follower = 0,
    Candidate,
    Leader,
};
