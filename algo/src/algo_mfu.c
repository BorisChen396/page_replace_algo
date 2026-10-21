#include <stdlib.h>

#include "algo.h"


size_t frames_count;
uint32_t *counters;

void algo_internal_init(algo_page_entry_t *frame_pool, size_t frame_pool_size) {
    if (counters) {
        algo_internal_deinit();
    }
    frames_count = 0;
    counters = calloc(frame_pool_size, sizeof(uint32_t));
}

void algo_internal_deinit() {
    frames_count = 0;
    free(counters);
    counters = NULL;
}

bool algo_internal_trap_hit(algo_page_entry_t *frame_pool, size_t hit_idx) {
    // Prevent counters from overflowing
    if (counters[hit_idx] < 0xffffffff) counters[hit_idx]++;
}

algo_page_access_result_t algo_internal_page_fault(algo_page_entry_t page_entry, algo_page_entry_t *frame_pool, size_t frame_pool_size) {
    // Check if the request page exists and find the page with the largest count at the same time.
    size_t max_page_idx = 0;
    for (size_t i = 0; i < frame_pool_size; i++) {
        algo_page_entry_t *entry = &frame_pool[i];
        if (counters[max_page_idx] < counters[i])
            max_page_idx = i;
    }
    algo_page_access_result_t result = {
        .disk_writes = 0,
        .page_fault = 1
    };

    if (frames_count < frame_pool_size) {
        // There are still free frames. Just use it.
        frame_pool[frames_count] = page_entry;
        counters[frames_count] = 1;
        frames_count++;
        return result;
    }
    else {
        if (frame_pool[max_page_idx].dirty) {
            // The removed page is a dirty page. Need to write back.
            result.disk_writes++;
        }
        frame_pool[max_page_idx] = page_entry;
        counters[max_page_idx] = 1;
        return result;
    }
}
