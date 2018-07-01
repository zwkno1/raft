#pragma once

#include "common.h"

class Database
{
public:
    virtual NodeState loadNodeState() = 0;

    virtual void saveNodeState(const NodeState & state) = 0;

    virtual ~Database() {}
};

typedef shared_ptr<Database> DatabasePtr;

