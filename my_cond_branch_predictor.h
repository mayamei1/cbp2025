#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <array>
#include <stdlib.h>

using bht_entry_t = uint16_t;
using gbh_t = uint64_t;

static const size_t MAX_SIZE = 192000;
static const size_t SATURATION_BITS = 2;
static const size_t BRANCH_HISTORY_LENGTH = 1;

/* MAX_SIZE = NUM_HISTORY_PATTERNS * NUM_BHT_ENTRIES * SATURATION_BITS + BRANCH_HISTORY_LENGTH */
static const size_t NUM_HISTORY_PATTERNS = 1 << BRANCH_HISTORY_LENGTH;
static const gbh_t HISTORY_MASK = NUM_HISTORY_PATTERNS - 1;
static const size_t NUM_BHT_ENTRIES = (MAX_SIZE - BRANCH_HISTORY_LENGTH) / (NUM_HISTORY_PATTERNS * SATURATION_BITS);

static const bht_entry_t SATURATION_VALUE = (1 << SATURATION_BITS) - 1;
static const bht_entry_t NOT_TAKEN_MAX = (1 << (SATURATION_BITS - 1)) - 1;

class BHT {
    private:
        /* NUM_BHT_ENTRIES * SATURATION_BITS */
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
        /* BRANCH_HISTORY_LENGTH */
        gbh_t GBH;
        /* NUM_HISTORY_PATTERNS * NUM_BHT_ENTRIES * SATURATION_BITS */
        std::array<BHT, NUM_HISTORY_PATTERNS> BHTs;
    public:
        void setup() {
            GBH = 0;
            for (BHT &b : BHTs) b.setup();
        }
        bool predict(int idx) {
            return BHTs[GBH].predict(idx);
        }
        void update(int idx, bool val) {
            BHTs[GBH].update(idx, val);
            GBH = ((GBH << 1) | val) & HISTORY_MASK;
        }
};

/*
====================================================================================================
    Size of Predictor: 192 KB
    NUM_HISTORY_PATTERNS * NUM_BHT_ENTRIES * SATURATION_BITS + BRANCH_HISTORY_LENGTH
====================================================================================================
*/
class gselect
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
static gselect predictor;
