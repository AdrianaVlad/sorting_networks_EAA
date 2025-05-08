#pragma once

#include "RuntimeNetwork.h"
#include "NetworkGenerator.h"
#include "WorkingList.h"

class NetworkRemover {
private:
    NetworkGenerator* generator_;
    RuntimeNetwork* net_;
    WorkingList* workList_;

public:
    NetworkRemover(NetworkGenerator* generator, RuntimeNetwork* net);

    int operator()();
    WorkingList* getWorkList() const { return workList_; }
};
