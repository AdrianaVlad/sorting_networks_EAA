#include "GreenFilter.h"

GreenFilter::GreenFilter(int nbWires) : Network(nbWires) {
    init();
}

void GreenFilter::init() {
    int len = 1;
    while (len < nbWires_) {
        for (int k = 0; k < len; ++k) {
            for (int i = k; i + len < nbWires_; i += 2 * len) {
                addComparator(i, i + len);
            }
        }
        len *= 2;
    }

    outputSet();
}
