#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <memory>

enum NodeType
{
    Follower = 0,
    Candidate,
    Leader,
};

typedef std::uint64_t Term;
typedef std::uint64_t NodeId;
typedef std::uint64_t Index;
typedef std::string Value;

struct LogEntry
{
    Term term;
    Index index;
    Value value;
};
