#ifndef BPTREE_H
#define BPTREE_H

#include <stdint.h>
#include <stdbool.h>

#define BPTREE_ORDER 4  // ordem da árvore (nº máximo de filhos por nó interno)
#define MAX_KEYS 3        // até 3 chaves por nó
#define MAX_CHILDREN 4    // até 4 filhos (chaves + 1)


typedef struct BPTreeNode {
    bool is_leaf;
    int num_keys;
    int keys[MAX_KEYS];
    void *values[MAX_KEYS];         // só se for folha
    struct BPTreeNode *children[MAX_KEYS + 1];
    struct BPTreeNode *next;        // ponteiro para próxima folha
} BPTreeNode;

BPTreeNode* bptree_insert(BPTreeNode *root, int key, void *value);
void* bptreesearch(BPTreeNode *node, int key);
void bptree_print(BPTreeNode *root);

#endif