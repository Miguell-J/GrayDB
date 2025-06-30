#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>

#define HASH_TABLE_SIZE 1024

typedef struct HashNode {
    char *key;
    void *value;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_TABLE_SIZE];
} HashTable;

HashTable* create_table();
void ht_insert(HashTable *table, const char *key, void *value);
void* ht_lookup(HashTable *table, const char *key);
bool ht_delete(HashTable *table, const char *key);
void free_table(HashTable *table);
int hash(const char *key);

#endif

