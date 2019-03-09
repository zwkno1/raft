#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <memory>
#include <chrono>
#include <random>

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

typedef std::chrono::steady_clock Clock;
typedef std::chrono::milliseconds Duration;
typedef std::chrono::steady_clock::time_point TimePoint;
using namespace std::literals;

struct LogEntry
{
    Term term;
    Index index;
    Value value;
};
