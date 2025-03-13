#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdlib.h>
#include <map>
#include <array>

#ifndef MAX_PRED_BITS
#define MAX_PRED_BITS 192000
#endif

#ifndef NUM_BHR_INDEX_BITS
#define NUM_BHR_INDEX_BITS 12
#endif

#ifndef NUM_SATURATION_BITS
#define NUM_SATURATION_BITS 2
#endif

#ifndef BRANCH_HISTORY_LENGTH
#define BRANCH_HISTORY_LENGTH 4
#endif

#define BHR_SIZE (1 << NUM_BHR_INDEX_BITS)
#define BHR_INDEX_MASK (BHR_SIZE - 1)
#define MAX_SATURATION_VALUE ((1 << NUM_SATURATION_BITS) - 1)
#define MAX_NOT_TAKEN (MAX_SATURATION_VALUE >> 1)
#define BRANCH_HISTORY_SIZE (1 << BRANCH_HISTORY_LENGTH)
#define BRANCH_HISTORY_MASK (BRANCH_HISTORY_SIZE - 1)

class MyPredictor {
    private:
    std::array<std::array<uint8_t, BHR_SIZE>, BRANCH_HISTORY_SIZE> bhr;
    std::array<uint16_t, BHR_SIZE> lbh;
    public:
    void setup() {
        // Check # of bits used
        int bhr_count = BRANCH_HISTORY_SIZE * BHR_SIZE * NUM_SATURATION_BITS;
        int lbh_count = BHR_SIZE * BRANCH_HISTORY_LENGTH;
        assert((bhr_count + lbh_count) <= MAX_PRED_BITS);
        
        // BHR initialization
        for (auto b : bhr) b.fill(MAX_NOT_TAKEN);
    }

    bool predict(uint64_t pc) {
        // Calculate index
        int index = (pc >> 2) & BHR_INDEX_MASK;
        bool pred = bhr[lbh[index]][index] > MAX_NOT_TAKEN;
        return pred;
    }

    void update(uint64_t pc, bool taken) {
        // Calculate index
        int index = (pc >> 2) & BHR_INDEX_MASK;
        if (taken && bhr[lbh[index]][index] != MAX_SATURATION_VALUE) bhr[lbh[index]][index]++;
        else if (!taken && bhr[lbh[index]][index] != 0) bhr[lbh[index]][index]--;
        lbh[index] = ((lbh[index] << 1) | taken) & BRANCH_HISTORY_MASK;
    }
};

#endif
static MyPredictor cond_predictor_impl;
