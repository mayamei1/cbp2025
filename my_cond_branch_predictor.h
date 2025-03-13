#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdlib.h>
#include <map>
#include <array>

#ifndef MAX_PRED_BITS
#define MAX_PRED_BITS 192000
#endif

#ifndef NUM_BHR_INDEX_BITS
#define NUM_BHR_INDEX_BITS 16
#endif

#ifndef NUM_SATURATION_BITS
#define NUM_SATURATION_BITS 2
#endif

#define BHR_SIZE (1 << NUM_BHR_INDEX_BITS)
#define BHR_INDEX_MASK (BHR_SIZE - 1)
#define MAX_SATURATION_VALUE ((1 << NUM_SATURATION_BITS) - 1)
#define MAX_NOT_TAKEN (MAX_SATURATION_VALUE >> 1)

class MyPredictor {
    private:
    std::array<uint8_t, BHR_SIZE> bhr;
    public:
    void setup() {
        // Check # of bits used
        int num_bits = BHR_SIZE * NUM_SATURATION_BITS;
        assert(num_bits <= MAX_PRED_BITS);
        
        // BHR initialization
        bhr.fill(MAX_NOT_TAKEN);
    }

    bool predict(uint64_t pc) {
        // Calculate index
        int index = (pc >> 2) & BHR_INDEX_MASK;
        bool pred = bhr[index] > MAX_NOT_TAKEN;
        return pred;
    }

    void update(uint64_t pc, bool taken) {
        // Calculate index
        int index = (pc >> 2) & BHR_INDEX_MASK;
        if (taken && bhr[index] != MAX_SATURATION_VALUE) bhr[index]++;
        else if (!taken && bhr[index] != 0) bhr[index]--;
    }
};

#endif
static MyPredictor cond_predictor_impl;
