#include <dpu.h>
#include <stdio.h>
#include <stdint.h>

#define DPU_BINARY "skip_list_dpu"
#define INSERT 0
#define SEARCH 1

#define NUM_COMMANDS 5

int main() {
    struct dpu_set_t dpu_set,dpu;
    DPU_ASSERT(dpu_alloc(1, NULL, &dpu_set));  
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));

    uint64_t commands[NUM_COMMANDS] = {INSERT, INSERT, INSERT, INSERT,SEARCH};
    uint64_t keys[NUM_COMMANDS] = {42, 10,5,1,5};
    uint64_t results[NUM_COMMANDS] ={0};

    DPU_ASSERT(dpu_copy_to(dpu_set, DPU_MRAM_HEAP_POINTER_NAME, 0, commands, sizeof(commands)));
    DPU_ASSERT(dpu_copy_to(dpu_set, DPU_MRAM_HEAP_POINTER_NAME, sizeof(commands), keys, sizeof(keys)));

    DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));


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