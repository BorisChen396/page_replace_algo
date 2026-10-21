#include <stdlib.h>

#include "algo.h"


static size_t frames_count, hand;

void algo_internal_init(algo_page_entry_t *frame_pool, size_t frame_pool_size) {
    frames_count = 0;
    hand = 0;
}

void algo_internal_deinit() {
    frames_count = 0;
    hand = 0;
}

bool algo_internal_trap_hit(algo_page_entry_t *frame_pool, size_t hit_idx) {
    return false;
}

algo_page_access_result_t algo_internal_page_fault(algo_page_entry_t page_entry, algo_page_entry_t *frame_pool, size_t frame_pool_size) {
    algo_page_access_result_t result = {
        .disk_writes = 0,
        .page_fault = 1
    };
    if (frames_count < frame_pool_size) {
        // There are still free frames. Just use it.
        frame_pool[frames_count] = page_entry;
        frames_count++;
        return result;
    }
    else {
        // Replace a page. (ref, dirty): (0, 0) -> (0, 1) -> (1, 0) -> (1, 1)
        size_t victim_idx = 0;
        while (true) {
            for (size_t i = 0; i < frames_count; i++) {
                size_t idx = (hand + i) % frame_pool_size;
                algo_page_entry_t *entry = &frame_pool[idx];
                if (!entry->reference && !entry->dirty) {
                    victim_idx = idx;
                    goto found;
                }
            }
            for (size_t i = 0; i < frames_count; i++) {
                size_t idx = (hand + i) % frame_pool_size;
                algo_page_entry_t *entry = &frame_pool[idx];
                if (!entry->reference && entry->dirty) {
                    victim_idx = idx;
                    goto found;
                }
                entry->reference = false;
            }
        }
found:
        algo_page_entry_t *victim = &frame_pool[victim_idx];
        hand = (victim_idx + 1) % frame_pool_size;
        if (victim->dirty) {
            // The removed page is a dirty page. Need to write back.
            result.disk_writes++;
        }
        *victim = page_entry;
        victim->reference = true;
        return result;
    }
}
