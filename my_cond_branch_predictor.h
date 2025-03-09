#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <array>
#include <stdlib.h>

using bht_entry_t = uint16_t;
using lbh_entry_t = uint16_t;

static const size_t MAX_SIZE = 192000;
static const size_t SATURATION_BITS = 2;
static const size_t BRANCH_HISTORY_LENGTH = 1;

static const size_t NUM_HISTORY_PATTERNS = 1 << BRANCH_HISTORY_LENGTH;
static const lbh_entry_t HISTORY_MASK = NUM_HISTORY_PATTERNS - 1;
static const size_t NUM_BHT_ENTRIES = MAX_SIZE / (SATURATION_BITS * NUM_HISTORY_PATTERNS);
static const bht_entry_t SATURATION_VALUE = (1 << SATURATION_BITS) - 1;
static const bht_entry_t NOT_TAKEN_MAX = (1 << (SATURATION_BITS - 1)) - 1;

class BHT {
    private:
        std::array<bht_entry_t, NUM_BHT_ENTRIES> table;
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

class TwoLevelBHT {
    private:
        std::array<lbh_entry_t, NUM_BHT_ENTRIES> LBH;
        std::array<BHT, NUM_HISTORY_PATTERNS> BHTs;
    public:
        void setup() {
            LBH.fill(0);
            for (BHT &b : BHTs) b.setup();
        }
        bool predict(int idx) {
            return BHTs[LBH[idx]].predict(idx);
        }
        void update(int idx, bool val) {
            BHTs[LBH[idx]].update(idx, val);
            LBH[idx] = ((LBH[idx] << 1) | val) & HISTORY_MASK;
        }
};

/* Size of Predictor: 192 KB */
class TwoLevelBranchPredictor
{
    TwoLevelBHT bht;
    public:
        void setup() {
            bht.setup();
        }

        bool predict(uint64_t pc) {
            return bht.predict((pc >> 2) % NUM_BHT_ENTRIES);
        }
        void update(uint64_t pc, bool taken) {
            bht.update((pc >> 2) % NUM_BHT_ENTRIES, taken);
        }
};
// =================
// Predictor End
// =================

#endif
static TwoLevelBranchPredictor predictor;
