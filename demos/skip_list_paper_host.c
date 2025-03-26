#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#define MAX_LEVEL 6
#define NUM_DPUS 4
#define UPPER_LEVELS 2


struct Node {
    int key;
    struct Node *left, *right, *up, *down;
};

struct SkipList {
    struct Node *header; 
    int level;  
};

struct DpuNode{  
    int key;
    int level;
    int left, right,down,up;
};

struct Node *createNode(int key) {
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->key = key;
    node->left = node->right = node->up = node->down = NULL;
    return node;
}

struct SkipList *createSkipList() {
    struct SkipList *list = (struct SkipList *)malloc(sizeof(struct SkipList));
    list->level = 0;
    list->header = createNode(INT_MIN);
    return list;
}

void serializeUpperLayer(struct SkipList *list,struct DpuNode *dpuNodes){
    struct Node * nodes[100];
    int nodeCount = 0;

    for(int level = list->level; level >= list->level - UPPER_LEVELS)

}

void insert(struct SkipList *list, int key) {
   struct Node *update[MAX_LEVEL];
   struct Node *current = list->header;

   

    for (int i = list->level; i >= 0; i--) {
        while (current->right && current->right->key < key)
            current = current->right;
        update[i] = current;
        current = current->down;
    }

    struct Node *new_node = createNode(key);
    new_node->right = update[0]->right;
    if (update[0]->right) update[0]->right->left = new_node;
    update[0]->right = new_node;
    new_node->left = update[0];

    int node_level = 0; 
    while (rand() < RAND_MAX / 2 && node_level < MAX_LEVEL) {
        node_level++;
        if (node_level > list->level) {
            list->level++;
            update[node_level] =  createNode(INT_MIN);
            list->header->up = update[node_level];
            update[node_level]-> down = list->header;
            list->header = update[node_level];
        
        }

        struct Node *upper_node = createNode(key);
        upper_node->down = new_node;
        new_node->up = upper_node;
        upper_node->right = update[node_level]->right;
        if (update[node_level]->right) update[node_level]->right->left = upper_node;
        update[node_level]->right = upper_node;
        upper_node->left = update[node_level];

        new_node = upper_node;
    }
    
}




void printSkipList(struct SkipList *list) {
    for (int i = list->level; i >= 0; i--) {
        struct Node *current = list->header;
        while (current->down) current = current->down;
        for (int j = 0; j < i; j++) current = current->up;
        printf("Level %d: ", i);
        while (current) {
            printf("%d -> ", current->key);
            current = current->right;
        }
        printf("NULL\n");
    }
}

int main(){
    struct SkipList *list = createSkipList();
    insert(list, 3);
    insert(list, 6);
    insert(list, 7);
    insert(list, 9);
    insert(list,15);
    insert(list,22);
    insert(list ,4);
    insert(list,1);


    printf("Skip List:\n");
    printSkipList(list);

    return 0;
}