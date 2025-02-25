#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mram.h>
#include <defs.h>
#include <alloc.h>
#include <stdint.h>


#define MAX_LEVEL 6
#define INSERT 0
#define SEARCH 1
#define MRAM_HEAP_SIZE (8 * 1024 * 1024)


#define NUM_COMMANDS 5

static uint32_t mram_heap_offset = 0;

__mram_ptr struct SkipList* initializeSkipList(void);
__mram_ptr struct Node* createNode(uint64_t,int);
int insert( __mram_ptr struct SkipList *, uint64_t);
int getRandomLevel(void);

__host uint64_t results[NUM_COMMANDS]={0};



#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 4294967296 


static uint32_t lcg_seed = 123456789;






uint32_t lcg_rand(void) {
    lcg_seed = (LCG_A * lcg_seed + LCG_C) % LCG_M; //Xn+1 =(a⋅X n +c) mod m
    return lcg_seed;
}


struct Node {
    uint64_t key;
    __mram_ptr struct Node *next[MAX_LEVEL];
};

struct SkipList {
    int level;
    __mram_ptr struct Node *head;
};

__mram_ptr void* mram_alloc(uint32_t size) {
    if (mram_heap_offset + size > MRAM_HEAP_SIZE) {
        return NULL; 
    }
   __mram_ptr void* ptr = (__mram_ptr void*)(DPU_MRAM_HEAP_POINTER + mram_heap_offset);
    mram_heap_offset += size;
    return ptr;
}

__mram_ptr struct Node* createNode(uint64_t key, int level){
    
   __mram_ptr struct Node* node = (__mram_ptr struct Node *)mram_alloc(sizeof(struct Node));
    node->key = key;
    for(int i = 0; i<level; i++){
        node->next[i] = NULL;
    }
    return node;
}

__mram_ptr struct SkipList* initializeSkipList(void){
    __mram_ptr struct SkipList* lst = (__mram_ptr struct SkipList *)mram_alloc(sizeof(struct SkipList));
    lst->level = 0;
    lst-> head = createNode(INT_MIN, MAX_LEVEL);
    
    return lst;
}


int getRandomLevel(void){
    int level = 0;
    while(lcg_rand()%2 && level < MAX_LEVEL){
        level ++;
    }
        
    return level;
}


int insert(__mram_ptr struct SkipList * lst , uint64_t key){
    
    __mram_ptr struct Node *update[MAX_LEVEL];
    __mram_ptr struct Node *curr = lst->head;
    
    for (int i = lst->level; i >= 0; i--) {
        while (curr->next[i] != NULL && curr->next[i]->key < key) {
            curr = curr->next[i];
        }
        update[i] = curr;
    }
    
    int newLevel = getRandomLevel();
    
    if (newLevel > lst->level) {
        for (int i = lst->level + 1; i <= newLevel; i++) {
            update[i] = lst->head;
        }
        lst->level = newLevel;
    }
    
    __mram_ptr struct Node *newNode = createNode(key, newLevel+1);
    for (int i = 0; i <= newLevel; i++) {
            newNode->next[i] = update[i]->next[i];
            update[i]->next[i] = newNode;
    }

    return 0;
    
}


int search(__mram_ptr struct SkipList* lst, uint64_t key){
    __mram_ptr struct Node * curr = lst->head;
    
    for(int i = lst->level;i>=0;i--){
        while(curr->next[i] != NULL && curr->next[i]->key < key){
            curr = curr->next[i];
        }
    }
    
    curr = curr->next[0];
    
    if(curr != NULL && curr->key ==key){
        return 1;
    }
    return 0;
}


void display(struct SkipList *lst) {
    printf("\nSkip List Structure:\n");
    for (int i = lst->level; i >= 0; i--) {
        __mram_ptr struct Node *curr = lst->head->next[i];
        printf("Level %d: ", i);
        while (curr!= NULL) {
            printf("%lu -> ", curr->key);
            curr = curr->next[i];
        }
        printf("NULL\n");
    }
}

// void process_command(__mram_ptr struct SkipList *list, __mram_ptr uint64_t *commands, __mram_ptr uint64_t *keys,uint64_t* results,int num_commands) {
//     for (int i = 0; i < num_commands; i++) {
//         uint64_t command, key, result;
//         mram_read(&commands[i], &command, sizeof(command));
//         mram_read(&keys[i], &key, sizeof(key));

//         switch (command) {
//             case INSERT:
//                 result = insert(list, key);
//                 break;
//             case SEARCH:
//                 result = search(list, key);
//                 break;
//             case DELETE:
//                 result = delete(list, key);
//                 break;
//             default:
//                 result = -1; // Invalid command
//         }

//         mram_write(&result, &results[i], sizeof(result));
//     }
// }

int main() {
    uint64_t commands[NUM_COMMANDS];
    uint64_t keys[NUM_COMMANDS];
    

    mram_read((__mram_ptr void*)(DPU_MRAM_HEAP_POINTER), commands, sizeof(commands));
    mram_read((__mram_ptr void*)(DPU_MRAM_HEAP_POINTER + sizeof(commands)), keys, sizeof(keys));
 
   __mram_ptr struct SkipList *list = initializeSkipList();

    for (int i = 0; i < NUM_COMMANDS; i++) {
        switch (commands[i]) {
            case INSERT:
                results[i] = insert(list, keys[i]);
                break;
            case SEARCH:
                results[i] = search(list, keys[i]);
                break;
            default:
                results[i] = -1; // Invalid command
        }
    }
  
  //mram_write(results, (__mram_ptr void*)(DPU_MRAM_HEAP_POINTER + sizeof(commands) + sizeof(keys)), sizeof(results));
    //mram_write(results, DPU_MRAM_HEAP_POINTER, sizeof(results));

    return 0;
}
