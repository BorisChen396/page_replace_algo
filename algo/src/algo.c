#include <stdlib.h>

#include "algo.h"


static algo_page_entry_t *frame_pool = NULL;
static size_t frame_pool_size = 0;

void algo_init(const size_t fps) {
    if (frame_pool) {
        free(frame_pool);
    }
    frame_pool_size = fps;
    frame_pool = calloc(frame_pool_size, sizeof(algo_page_entry_t));
    algo_internal_init(frame_pool, frame_pool_size);
}

void algo_deinit() {
    algo_internal_deinit();
    frame_pool_size = 0;
    free(frame_pool);
}

algo_page_access_result_t algo_access_page(algo_page_entry_t page_entry) {
    for (size_t i = 0; i < frame_pool_size; i++) {
        algo_page_entry_t *entry = &frame_pool[i];
        if (entry->page_num == page_entry.page_num) {
            entry->dirty |= page_entry.dirty;
            entry->reference = true;
            return (algo_page_access_result_t) {
                .disk_writes = 0,
                .page_fault = 0,
                .interrupt = algo_internal_trap_hit(frame_pool, i) ? 1 : 0
            };
        }
    }
    return algo_internal_page_fault(page_entry, frame_pool, frame_pool_size);
}
