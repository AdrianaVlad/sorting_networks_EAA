#include "NetworkRemover.h"
#include <mutex>

NetworkRemover::NetworkRemover(NetworkGenerator* generator, RuntimeNetwork* net)
    : generator_(generator), net_(net), workList_(generator->getWorkList()) {}

int NetworkRemover::operator()() {
    if (net_->dead) {
        return 0;
    }

    int removed = 0;
    int first = net_->outSize;
    int last = workList_->last();

    for (int i = first; i <= last; ++i) {
        int n = workList_->networkList(i)->size();
        for (int j = 0; j < n; ++j) {
            RuntimeNetwork* other = workList_->getNetwork(i, j);

            if (net_->dead) {
                return removed;
            }

            if (other == net_ || other->dead ||
                (other->id <= net_->checkedSubsumesId) ||
                (net_->id <= other->checkedSubsumedById)) {
                continue;
            }

            if (!net_->subsumes(other)) {
                continue;
            }

            {
                std::lock_guard<std::recursive_mutex> lock(workList_->getMutex());
                if (!net_->dead) {
                    workList_->addDead(other);
                    removed++;
                }
            }
        }
    }

    return removed;
}
