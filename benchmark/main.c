#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include "algo.h"


int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s FRAME_COUNT REF_SEQ_TXT\n", argv[0]);
        return 0;
    }

    char *frame_count_endptr;
    uint64_t frame_count = strtoul(argv[1], &frame_count_endptr, 10);
    if (frame_count_endptr == argv[1]) {
        fprintf(stderr, "Error: Invalid argument. (FRAME_COUNT = \"%s\")\n", argv[1]);
        return -1;
    }
    else if (*frame_count_endptr != '\0') {
        fprintf(stderr, "Warning: Trailing characters detected: %s\n", frame_count_endptr);
    }

    FILE *input_file;
    if (strcmp("-", argv[2]) == 0) {
        // Read the sequence from standard input.
        input_file = stdin;
    }
    else {
        // Read the sequence from a file.
        input_file = fopen(argv[2], "r");
    }
    if (input_file == NULL) {
        fprintf(stderr, "Failed to open the input file!\n");
        return -1;
    }

    uint64_t disk_writes = 0, page_fault = 0;
    uint16_t page_num, reference, dirty;
    algo_init(frame_count);
    while (fscanf(input_file, "%"SCNu16" %"SCNu16" %"SCNu16, &page_num, &reference, &dirty) == 3) {
        algo_page_access_result_t req_result = algo_access_page((algo_page_entry_t) {
            .page_num = page_num,
            .reference = reference,
            .dirty = dirty
        });
        disk_writes += req_result.disk_writes;
        if (req_result.page_fault) page_fault++;
    }
    printf("%"PRIu64"\t%"PRIu64"\n", disk_writes, page_fault);
    algo_deinit();

    fclose(input_file);
    return 0;
}
