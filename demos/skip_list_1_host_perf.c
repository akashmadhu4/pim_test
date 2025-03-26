#define _POSIX_C_SOURCE 200809L
#include <dpu.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define DPU_BINARY "skip_list_dpu_perf"
#define INSERT 0
#define SEARCH 1
#define DELETE 2

#define NUM_COMMANDS 6

static inline double my_clock(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (1.0e-9 * t.tv_nsec + t.tv_sec);
}

int main() {
    struct dpu_set_t dpu_set,dpu;
    DPU_ASSERT(dpu_alloc(1, NULL, &dpu_set));  
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));

    uint64_t commands[NUM_COMMANDS] = {INSERT, INSERT, INSERT, INSERT,INSERT, SEARCH};
    uint64_t keys[NUM_COMMANDS] = {42, 20,5,1,10,5};
    uint64_t results[NUM_COMMANDS] ={0};

    DPU_ASSERT(dpu_copy_to(dpu_set, DPU_MRAM_HEAP_POINTER_NAME, 0, commands, sizeof(commands)));
    DPU_ASSERT(dpu_copy_to(dpu_set, DPU_MRAM_HEAP_POINTER_NAME, sizeof(commands), keys, sizeof(keys)));

    double start = my_clock();
    DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
    double end = my_clock();

    uint32_t nb_cycles;
    uint32_t clocks_per_sec;
    DPU_FOREACH(dpu_set, dpu) {
        DPU_ASSERT(dpu_copy_from(dpu, "nb_cycles", 0, &nb_cycles, sizeof(uint64_t)));
        DPU_ASSERT(dpu_copy_from(dpu, "CLOCKS_PER_SEC", 0, &clocks_per_sec, sizeof(uint32_t)));
    }
  
    printf("DPU cycles: %u\n", nb_cycles);
    printf("DPU time: %.2e secs.\n", (double)nb_cycles / clocks_per_sec);
  
    printf("Host elapsed time: %.2e secs.\n", end - start);

    //DPU_ASSERT(dpu_copy_from(dpu_set, DPU_MRAM_HEAP_POINTER_NAME,0, &results, sizeof(results)));
     DPU_FOREACH(dpu_set, dpu){
        DPU_ASSERT(dpu_copy_from(dpu, "results",0, results, sizeof(results)));
    }
    for (int i = 0; i < NUM_COMMANDS; i++) {
        printf("Command %lu, Key %lu: Result = %lu\n", commands[i], keys[i], results[i]);
    }

    DPU_ASSERT(dpu_free(dpu_set));
    return 0;
}