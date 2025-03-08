#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <array>
#include <stdlib.h>

using bht_t = uint16_t;

static const size_t MAX_SIZE = 192000;
static const size_t SATURATION_BITS = 2;
static const size_t BHT_SIZE = MAX_SIZE / SATURATION_BITS;
static const bht_t SATURATION_VALUE = (1 << SATURATION_BITS) - 1;
static const bht_t NOT_TAKEN_MAX = (1 << (SATURATION_BITS - 1)) - 1;

class BHT {
    private:
        std::array<bht_t, BHT_SIZE> table;
    public:
        void setup() {
            table.fill(NOT_TAKEN_MAX);
        }
        bool predict(int idx) {
            return table[idx] > NOT_TAKEN_MAX;
        }
        void update(int idx, bool val) {
            if (val && table[idx] != SATURATION_VALUE) table[idx]++;
            else if (!val && table[idx] != 0) table[idx]--;
        }
};

/* Size of Predictor: 192 KB */
class BitSaturatingBranchPredictor
{
    BHT bht;
    public:
        void setup() {
            bht.setup();
        }

        bool predict(uint64_t pc) {
            return bht.predict((pc >> 2) % BHT_SIZE);
        }

        void update(uint64_t pc, bool taken) {
            bht.update((pc >> 2) % BHT_SIZE, taken);
        }
};
// =================
// Predictor End
// =================

#endif
static BitSaturatingBranchPredictor predictor;
