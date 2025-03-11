#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <bitset>
#include <stdlib.h>

static const size_t MAX_SIZE = 192000;
static const size_t SATURATION_BITS = 1;
static const size_t BHT_SIZE = MAX_SIZE / SATURATION_BITS;
class BHT {
    private:
        std::bitset<BHT_SIZE> table;
    public:
        void setup() { table.reset(); }
        bool predict(int idx) { return table[idx]; }
        void update(int idx, bool val) { table.set(idx, val); }
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
