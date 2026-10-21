#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "algo.h"


static size_t frames_count, frames_start;

void algo_internal_init(algo_page_entry_t *frame_pool, size_t frame_pool_size) {
    frames_count = 0;
    frames_start = 0;
}

void algo_internal_deinit() {
    frames_count = 0;
    frames_start = 0;
}

bool algo_internal_trap_hit(algo_page_entry_t *frame_pool, size_t hit_idx) {
    return false;
}

algo_page_access_result_t algo_internal_page_fault(algo_page_entry_t page_entry, algo_page_entry_t *frame_pool, size_t frame_pool_size) {
    algo_page_access_result_t result = {
        .disk_writes = 0,
        .page_fault = 1,
        .interrupt = 0
    };
    if (frames_count >= frame_pool_size) {
        if (frame_pool[frames_start].dirty) {
            // The removed page is a dirty page. Need to write back.
            result.disk_writes++;
        }
        frames_start = (frames_start + 1) % frame_pool_size;
        frames_count--;
    }
    size_t rear = (frames_start + frames_count) % frame_pool_size;
    frame_pool[rear] = page_entry;
    frames_count++;
    
    return result;
}
