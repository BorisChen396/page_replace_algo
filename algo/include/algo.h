#pragma once

#include <stddef.h>
#include <stdint.h>


typedef struct algo_page_entry_t {
    uint16_t page_num;
    bool reference, dirty;
} algo_page_entry_t;

typedef struct algo_page_access_result_t {
    bool page_fault;
    uint8_t disk_writes, interrupt;
} algo_page_access_result_t;


void algo_init(size_t frame_pool_size);

void algo_deinit();

algo_page_access_result_t algo_access_page(algo_page_entry_t page_entry);

void algo_internal_init(algo_page_entry_t *frame_pool, size_t frame_pool_size);

void algo_internal_deinit();

bool algo_internal_trap_hit(algo_page_entry_t *frame_pool, size_t hit_idx);

algo_page_access_result_t algo_internal_page_fault(algo_page_entry_t page_entry, algo_page_entry_t *frame_pool, size_t frame_pool_size);
