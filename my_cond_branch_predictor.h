#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdlib.h>
#include <map>
#include <array>

#ifndef MAX_PRED_BITS
#define MAX_PRED_BITS 192000
#endif

class MyPredictor {
    public:
    void setup() {

    }

    bool predict(uint64_t pc) {
        return false;
    }

    void update(uint64_t pc, bool taken) {

    }
};

#endif
static MyPredictor cond_predictor_impl;
