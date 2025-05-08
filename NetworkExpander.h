#pragma once

#include "RuntimeNetwork.h"
#include "WorkingList.h"
#include "NetworkGenerator.h"


class NetworkExpander {
public:
    NetworkExpander(NetworkGenerator* generator, RuntimeNetwork* net);
    int operator()();

private:
    RuntimeNetwork* net_;
    NetworkGenerator* generator_;
    WorkingList* workList_;

    int expandAll();
    bool isSubsumed(RuntimeNetwork* net);
    void removeSubsumed(RuntimeNetwork* net);
    bool isRedundant(RuntimeNetwork* net, int wire0, int wire1);
};
