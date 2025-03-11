#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <array>
#include <stdlib.h>

using bht_entry_t = uint16_t;
using gbh_t = uint64_t;

static const size_t MAX_SIZE = 192000;
static const size_t SATURATION_BITS = 2;
static const size_t BRANCH_HISTORY_LENGTH = 16;

/* MAX_SIZE = NUM_HISTORY_PATTERNS * NUM_BHT_ENTRIES * SATURATION_BITS + BRANCH_HISTORY_LENGTH */
static const gbh_t HISTORY_MASK = (1 << BRANCH_HISTORY_LENGTH) - 1;
static const size_t NUM_BHT_ENTRIES = (MAX_SIZE - BRANCH_HISTORY_LENGTH) / SATURATION_BITS;

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

/*
====================================================================================================
    Size of Predictor: 192 KB
    NUM_BHT_ENTRIES * SATURATION_BITS + BRANCH_HISTORY_LENGTH
====================================================================================================
*/
class gshare
{
    /* NUM_BHT_ENTRIES * SATURATION_BITS */
    BHT bht;
    /* BRANCH_HISTORY_LENGTH */
    gbh_t gbh;
    private:
        int get_index(uint64_t pc) {
            return ((pc >> 2) % NUM_BHT_ENTRIES) ^ gbh;
        }
    public:
        void setup() {
            bht.setup();
            gbh = 0;
        }

        bool predict(uint64_t pc) {
            return bht.predict(get_index(pc));
        }
        void update(uint64_t pc, bool taken) {
            bht.update(get_index(pc), taken);
            gbh = ((gbh << 1) | taken) & HISTORY_MASK;
        }
};
// =================
// Predictor End
// =================

#endif
static gshare predictor;
