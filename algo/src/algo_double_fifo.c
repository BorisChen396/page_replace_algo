#include <stdio.h>
#include <stdlib.h>

#include "algo.h"


typedef struct {
    algo_page_entry_t *entries;
    size_t queue_size, entry_count, queue_front;
} algo_page_entry_queue_t;

static algo_page_entry_queue_t pre_queue, main_queue;
static uint32_t pre_queue_evict_counter, main_queue_evict_counter;

static void queue_init(algo_page_entry_queue_t *queue, algo_page_entry_t *queue_start, size_t queue_size) {
    queue->entries = queue_start;
    queue->queue_size = queue_size;
    queue->entry_count = 0;
    queue->queue_front = 0;
}

static void queue_free(algo_page_entry_queue_t *queue) {
    queue->entries = NULL;
    queue->queue_size = 0;
    queue->entry_count = 0;
    queue->queue_front = 0;
}

static algo_page_entry_t *queue_search(algo_page_entry_queue_t *queue, uint16_t page_num) {
    algo_page_entry_t *entry = NULL;
    for (size_t i = 0; i < queue->entry_count; i++) {
        algo_page_entry_t *e = &queue->entries[(queue->queue_front + i) % queue->queue_size];
        if (e->page_num == page_num) {
            entry = e;
        }
    }
    return entry;
}

static bool queue_is_full(algo_page_entry_queue_t *queue) {
    return queue->entry_count >= queue->queue_size;
}

static algo_page_entry_t queue_pop(algo_page_entry_queue_t *queue) {
    algo_page_entry_t removed = queue->entries[queue->queue_front];
    queue->queue_front = (queue->queue_front + 1) % queue->queue_size;
    queue->entry_count--;
    return removed;
}

static void queue_insert(algo_page_entry_queue_t *queue, algo_page_entry_t entry) {
    size_t rear = (queue->queue_front + queue->entry_count) % queue->queue_size;
    queue->entries[rear] = entry;
    queue->entry_count++;
}

void algo_internal_init(algo_page_entry_t *frame_pool, size_t frame_count) {
    algo_internal_deinit();
    if (frame_count < 4) {
        queue_init(&pre_queue, NULL, 0);
        queue_init(&main_queue, frame_pool, frame_count);
    }
    else {
        size_t pre_size  = frame_count - (frame_count >> 2);
        size_t main_size = frame_count >> 2;
        queue_init(&pre_queue, frame_pool, pre_size);
        queue_init(&main_queue, frame_pool + pre_size, main_size);
    }
    pre_queue_evict_counter = 0;
    main_queue_evict_counter = 0;
}

void algo_internal_deinit() {
    queue_free(&pre_queue);
    queue_free(&main_queue);
}

bool algo_internal_trap_hit(algo_page_entry_t *frame_pool, size_t hit_idx) {
    return false;
}

algo_page_access_result_t algo_internal_page_fault(algo_page_entry_t page_entry, algo_page_entry_t *frame_pool, size_t frame_pool_size) {
    if (pre_queue.queue_size < 1) {
        // Frame size is too small! Operate as a FIFO.

        // Requested page does not exist. Do FIFO.
        algo_page_access_result_t result = {
            .disk_writes = 0,
            .page_fault = 1
        };
        if (queue_is_full(&main_queue)) {
            algo_page_entry_t removed_frame = queue_pop(&main_queue);
            if (removed_frame.dirty) {
                result.disk_writes++;
            }
        }
        queue_insert(&main_queue, page_entry);

        return result;
    }
    else {
        algo_page_access_result_t result = {
            .disk_writes = 0,
            .page_fault = 1
        };
        if (queue_is_full(&pre_queue)) {
            algo_page_entry_t victim = queue_pop(&pre_queue);
            if (victim.reference) {
                if (queue_is_full(&main_queue)) {
                    while (true) {
                        algo_page_entry_t candidate = queue_pop(&main_queue);
                        if (candidate.reference) {
                            candidate.reference = false;
                            queue_insert(&main_queue, candidate);
                        }
                        else {
                            if (candidate.dirty) {
                                result.disk_writes++;
                            }
                            break;
                        }
                    }
                }
                victim.reference = false;
                queue_insert(&main_queue, victim);
            }
            else {
                if (victim.dirty) {
                    result.disk_writes++;
                }
            }
        }
        page_entry.reference = false;
        queue_insert(&pre_queue, page_entry);
        return result;
    }
}
