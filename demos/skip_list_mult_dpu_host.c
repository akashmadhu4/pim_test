#define _POSIX_C_SOURCE 200809L
#include <dpu.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define DPU_BINARY "skip_list_dpu_multi_dpu"
#define INSERT 0
#define SEARCH 1
#define DELETE 2

#define NUM_COMMANDS 6
#define NUM_DPUS 4



unsigned int hash_fn(int key){
    return (unsigned int)(key % NUM_DPUS);
}

const char* get_command_name(uint64_t command) {
    switch (command) {
        case INSERT: return "INSERT";
        case SEARCH: return "SEARCH";
        case DELETE: return "DELETE";
        default: return "UNKNOWN";
    }
}


int main() {
    struct dpu_set_t dpu_set,dpu;
    DPU_ASSERT(dpu_alloc(NUM_DPUS, NULL, &dpu_set));  
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));

    uint64_t commands[NUM_COMMANDS] = {INSERT, INSERT, INSERT, INSERT,INSERT, SEARCH};
    uint64_t keys[NUM_COMMANDS] = {42, 20,5,3,10,5};
    uint64_t results[NUM_COMMANDS] ={0};

    uint64_t dpu_commands[NUM_DPUS][NUM_COMMANDS];
    uint64_t dpu_keys[NUM_DPUS][NUM_COMMANDS];
    uint32_t dpu_command_count[NUM_DPUS] = {0};

    for (int i = 0; i < NUM_COMMANDS; i++) {

        uint64_t key = keys[i];
        uint64_t command = commands[i];
        unsigned int dpu_id = hash_fn(key);

        dpu_commands[dpu_id][dpu_command_count[dpu_id]] = command;
        dpu_keys[dpu_id][dpu_command_count[dpu_id]] = key;
        dpu_command_count[dpu_id]++;
    }

    for (int dpu_id = 0; dpu_id < NUM_DPUS; dpu_id++) {
        printf("DPU %d:\n", dpu_id);
        for (int j = 0; j < dpu_command_count[dpu_id]; j++) {
            printf("  Operation: %s, Key: %lu\n", get_command_name(dpu_commands[dpu_id][j]), dpu_keys[dpu_id][j]);
        }
    }

    uint32_t dpu_index = 0;
    DPU_FOREACH(dpu_set, dpu) {
        DPU_ASSERT(dpu_copy_to(dpu_set, DPU_MRAM_HEAP_POINTER_NAME, 0, dpu_commands[dpu_index], sizeof(dpu_commands[0])));
        DPU_ASSERT(dpu_copy_to(dpu_set, DPU_MRAM_HEAP_POINTER_NAME,sizeof(dpu_commands[0]) , dpu_keys[dpu_index], sizeof(dpu_keys[0])));
        dpu_index++;
    }
    // uint32_t dpu_index = 0;
    // DPU_FOREACH(dpu_set, dpu) {
    //     if (dpu_command_count[dpu_index] > 0) {
          
    //         DPU_ASSERT(dpu_prepare_xfer(dpu, dpu_commands[dpu_index]));
    //         DPU_ASSERT(dpu_prepare_xfer(dpu, dpu_keys[dpu_index]));
    //     }
    //     dpu_index++;
    // }
    // DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, sizeof(dpu_commands[0]), DPU_XFER_DEFAULT));
    // DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, sizeof(dpu_commands[0]), sizeof(dpu_keys[0]), DPU_XFER_DEFAULT));

   
    DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));

    uint64_t all_results[NUM_DPUS][NUM_COMMANDS] = {0};
    // dpu_index = 0;
    // DPU_FOREACH(dpu_set, dpu) {
    //     if (dpu_command_count[dpu_index] > 0) {
    //         DPU_ASSERT(dpu_prepare_xfer(dpu, all_results[dpu_index]));
    //     }
    //     dpu_index++;
    // }
    // DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU,"results", 0, sizeof(all_results[0]), DPU_XFER_DEFAULT));
    dpu_index = 0;
    DPU_FOREACH(dpu_set, dpu) {
        DPU_ASSERT(dpu_copy_from(dpu, "results", 0, &all_results[dpu_index], sizeof(all_results[0])));
        dpu_index++;
    }

    int offset = 0;
    for (int i = 0; i < NUM_DPUS; i++) {
        for (int j = 0; j < dpu_command_count[i]; j++) {
            results[offset + j] = all_results[i][j];
        }
        offset += dpu_command_count[i];
    }
    
    for (int i = 0; i < NUM_COMMANDS; i++) {
        printf("Command %s, Key %lu: Result = %lu\n", get_command_name(commands[i]), keys[i], results[i]);
    }

    DPU_ASSERT(dpu_free(dpu_set));
    return 0;
}