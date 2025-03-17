#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdlib.h>
#include <map>
#include <array>
#include <vector>

#ifndef MAX_PRED_BITS
#define MAX_PRED_BITS 192000
#endif

#ifndef NUM_BHR_INDEX_BITS
#define NUM_BHR_INDEX_BITS 16
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

#define OBQ_SIZE 1024
#define NUM_OBQ_INDEX_BITS 10

using branch_hist_t = uint16_t;

template<typename T, std::size_t size> class cbuffer {
    private:
    std::vector<T> buffer;
    int head, tail;

    int minc(int& t) {
        int ret = t;
        t = (t + 1) % size;
        return ret;
    }
    public:
    cbuffer() {
        buffer.assign(size, 0);
        head = tail = 0;
    }

    int push(T t) {
        buffer[tail] = t;
        return minc(tail);
    }

    T pop() {
        return buffer[minc(head)];
    }

    T at(int index) {
        return buffer[index];
    }

    T set_tail(int index) {
        tail = index;
        return buffer[tail];
    }
};

class MyPredictor {
    private:
    std::array<uint8_t, BHR_SIZE> bhr;
    branch_hist_t gbh;

    cbuffer<branch_hist_t, OBQ_SIZE> obq;
    std::map<uint64_t, int> obq_id;

    uint64_t get_unique_inst_id(uint64_t seq_no, uint8_t piece) {
        return (seq_no << 4) | (piece & 0x000F);
    }

    int get_index(uint64_t pc, branch_hist_t bh) {
        // (PC mod BHR size) XOR (Left-aligned GBH)
        return ((pc >> 2) & BHR_INDEX_MASK) ^ (bh << (NUM_BHR_INDEX_BITS - BRANCH_HISTORY_LENGTH));
    }

    void update_hist(branch_hist_t &bh, bool taken) {
        bh = ((bh << 1) | taken) & BRANCH_HISTORY_MASK;
    }

    public:
    void setup() {
        // Check # of bits used
        int bhr_count = BHR_SIZE * NUM_SATURATION_BITS;
        int gbh_count = BRANCH_HISTORY_LENGTH;
        int obq_count = OBQ_SIZE * BRANCH_HISTORY_LENGTH + NUM_OBQ_INDEX_BITS;
        assert((bhr_count + gbh_count + obq_count) <= MAX_PRED_BITS);
        
        // BHR initialization
        bhr.fill(MAX_NOT_TAKEN);
        gbh = 0;

        printf("%d\n", BRANCH_HISTORY_LENGTH);
    }

    bool predict(uint64_t seq_no, uint8_t piece, uint64_t pc) {
        // Calculate index
        int bhr_index = get_index(pc, gbh);
        int inst_id = get_unique_inst_id(seq_no, piece);

        // Store old branch history to OBQ
        obq_id[inst_id] = obq.push(gbh);
        // Get prediction
        bool pred = bhr[bhr_index] > MAX_NOT_TAKEN;
        // Speculatively update branch history
        update_hist(gbh, pred);
        
        return pred;
    }

    void spec_fix(uint64_t seq_no, uint8_t piece, uint64_t pc, bool pred_dir, bool resolve_dir) {
        // Fixup only if incorrect prediction
        if (pred_dir == resolve_dir) return;
        // Calculate index/get OBQ index from pipeline
        int inst_id = get_unique_inst_id(seq_no, piece);
        int obq_index = obq_id[inst_id];

        // Fix branch history
        gbh = obq.set_tail(obq_index);
        update_hist(gbh, resolve_dir);
    }

    void update(uint64_t seq_no, uint8_t piece, uint64_t pc, bool taken) {
        // Calculate index
        int inst_id = get_unique_inst_id(seq_no, piece);
        int obq_index = obq_id[inst_id];
        // Get branch history right before this branch
        branch_hist_t br_hist = obq.at(obq_index);
        int bhr_index = get_index(pc, br_hist);
        // Increment/decrement with saturation
        uint8_t &counter = bhr[bhr_index];
        if (taken && counter != MAX_SATURATION_VALUE) counter++;
        else if (!taken && counter != 0) counter--;
    }

    void commit() {
        // Not really needed, as there is no reason to keep up with the head, but added for consistency
        obq.pop();
    }
};

#endif
static MyPredictor cond_predictor_impl;
