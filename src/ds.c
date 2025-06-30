#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdatomic.h>
#include <errno.h>
#include <readline/readline.h>

#include "../include/bptree.h"
#include "../include/hashtable.h"

BPTreeNode* bptree_insert_internal(BPTreeNode *node, int key, void *value,
    int *promoted_key, BPTreeNode **new_child);

BPTreeNode* split_leaf(BPTreeNode *leaf, int key, void *value,
int *promoted_key, BPTreeNode **new_leaf_out);

BPTreeNode* split_internal(BPTreeNode *node, int key, BPTreeNode *child,
int *promoted_key, BPTreeNode **new_child_out);

BPTreeNode* bptree_delete(BPTreeNode *root, int key);

// vamos começar aqui, vamos definir arvores, funções de consulta a arvores, e hash

// para arvores, vamos uasr b+tree

// e para hash vamos usar hash table com hash aberto

// primeiro vamos fazer a função para fazer a busca na arvore, passaremos o nó como um ponteiro, e depois a chave
// o algoritmo busca o nó na arvore, se for null, ele retorna NULL, caso o nó seja uma folha, ele itera por cada chave e então retorna os valores do nó
// caso o nó atual não seja o nó que procuramos, fazemos a procura recursivamente nos filhos do nó, até achar o nó desejado

void* bptreesearch(BPTreeNode *node, int key) {
    if (node == NULL) return NULL;

    if (node->is_leaf) {
        for (int i = 0; i < node->num_keys; i++) {
            if (node->keys[i] == key) {
                return node->values[i]; 
            }
        }
        return NULL;
    } else {
        for (int i = 0; i < node->num_keys; i++) {
            if (key < node->keys[i]) {
                return bptreesearch(node->children[i], key);
            }
        }
        return bptreesearch(node->children[node->num_keys], key);
    }
}

// já já faremos as outras funções de inserção, atualização e hash, mas iremos precisar de tables para isso, por isso iremos criar essa função primeiro
// Aqui a ideia é simples, olhamos dentro de uma key, dai para cada caractere, fazemos um hash

int hash(const char *key) {
    int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = (hash * 31 + key[i]) % HASH_TABLE_SIZE;
    }
    return hash;
}

// Show, agora que temos a função de hash, precismos de uma tabela para armazenar esses hashes
// alocamos memória, caso ela não exista, retornamos null

HashTable* create_table() {
    HashTable *table = malloc(sizeof(HashTable));
    if (!table) return NULL;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}

//agora precisamos da inserção dentro da hash table

void ht_insert(HashTable *table, const char *key, void *value) {
    int index = hash(key);
    HashNode *node = table->buckets[index];

    // atualiza valor se já existir
    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->value = value;
            return;
        }
        node = node->next;
    }

    // insere novo
    HashNode *new_node = malloc(sizeof(HashNode));
    new_node->key = strdup(key);  // cópia da chave
    new_node->value = value;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}


// agora devemos poder olhar dentro dela
void* ht_lookup(HashTable *table, const char *key) {
    int index = hash(key);
    HashNode *node = table->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

//devemos poder deletar também
bool ht_delete(HashTable *table, const char *key) {
    int index = hash(key);
    HashNode *node = table->buckets[index];
    HashNode *prev = NULL;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            if (prev)
                prev->next = node->next;
            else
                table->buckets[index] = node->next;

            free(node->key);
            free(node);
            return true;
        }
        prev = node;
        node = node->next;
    }

    return false;
}

// e então liberar memória da tabela
void free_table(HashTable *table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *node = table->buckets[i];
        while (node) {
            HashNode *next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }
    free(table);
}

// Perfeito! Agora que temos as ferramentas para utilizar hashes, podemos seguir com as implementações da arvore, como inserção, remoção, atualização e etc, vamos começar com inserção e criação de nó
// Aqui recebemos como argumento um true/flase, alocamos espaço para o novo nó, setamos as keys como zero, e o próximo nó como null, afinal ele é novo
BPTreeNode* bptree_create_node(bool is_leaf) {
    BPTreeNode *node = malloc(sizeof(BPTreeNode));
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;

    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// agora que podemos criar um novo nó, vamos criar a função pública de inserção
BPTreeNode* bptree_insert(BPTreeNode *root, int key, void *value) {
    if (root == NULL) {
        BPTreeNode *leaf = bptree_create_node(true);
        leaf->keys[0] = key;
        leaf->values[0] = value;
        leaf->num_keys = 1;
        return leaf;
    }

    void *promoted_value = NULL;
    int promoted_key = 0;
    BPTreeNode *new_child = NULL;

    BPTreeNode *new_root = bptree_insert_internal(root, key, value,
                                                  &promoted_key, 
                                                  &new_child);

    if (new_child != NULL) {
        BPTreeNode *new_root = bptree_create_node(false);
        new_root->keys[0] = promoted_key;
        new_root->children[0] = root;
        new_root->children[1] = new_child;
        new_root->num_keys = 1;
        return new_root;
    }

    return root;
}

// Ok, agora vamos criar uma função para inserção interna

BPTreeNode* bptree_insert_internal(BPTreeNode *node, int key, void *value,
                                   int *promoted_key, BPTreeNode **new_child) {
    int pos = 0;

    while (pos < node->num_keys && key >= node->keys[pos]) {
        pos++;
    }

    if (node->is_leaf) {
        if (node->num_keys < MAX_KEYS) {
            for (int i = node->num_keys; i > pos; i--) {
                node->keys[i] = node->keys[i-1];
                node->values[i] = node->values[i-1];
            }
            node->keys[pos] = key;
            node->values[pos] = value;
            node->num_keys++;
            *new_child = NULL;
            return NULL;
        } else {
            return split_leaf(node, key, value, promoted_key, new_child);
        }
    } else {
        int child_key;
        BPTreeNode *child_new = NULL;

        bptree_insert_internal(node->children[pos], key, value,
                               &child_key, &child_new);

        if (child_new == NULL) {
            *new_child = NULL;
            return NULL;
        }

        if (node->num_keys < MAX_KEYS) {
            for (int i = node->num_keys; i > pos; i--) {
                node->keys[i] = node->keys[i-1];
                node->children[i+1] = node->children[i];
            }
            node->keys[pos] = child_key;
            node->children[pos+1] = child_new;
            node->num_keys++;
            *new_child = NULL;
            return NULL;
        } else {
            return split_internal(node, child_key, child_new, promoted_key, new_child);
        }
    }
}



BPTreeNode* split_leaf(BPTreeNode *leaf, int key, void *value,
                       int *promoted_key, BPTreeNode **new_leaf_out) {
    BPTreeNode *new_leaf = bptree_create_node(true);
    int temp_keys[MAX_KEYS + 1];
    void *temp_values[MAX_KEYS + 1];

    for (int i = 0; i < MAX_KEYS; i++) {
        temp_keys[i] = leaf->keys[i];
        temp_values[i] = leaf->values[i];
    }

    int pos = 0;
    while (pos < MAX_KEYS && key > temp_keys[pos]) pos++;

    for (int i = MAX_KEYS; i > pos; i--) {
        temp_keys[i] = temp_keys[i-1];
        temp_values[i] = temp_values[i-1];
    }
    temp_keys[pos] = key;
    temp_values[pos] = value;

    int split = (MAX_KEYS + 1) / 2;

    leaf->num_keys = 0;
    for (int i = 0; i < split; i++) {
        leaf->keys[i] = temp_keys[i];
        leaf->values[i] = temp_values[i];
        leaf->num_keys++;
    }

    for (int i = split, j = 0; i < MAX_KEYS + 1; i++, j++) {
        new_leaf->keys[j] = temp_keys[i];
        new_leaf->values[j] = temp_values[i];
        new_leaf->num_keys++;
    }

    new_leaf->next = leaf->next;
    leaf->next = new_leaf;

    *promoted_key = new_leaf->keys[0];
    *new_leaf_out = new_leaf;
    return NULL;
}

BPTreeNode* split_internal(BPTreeNode *node, int key, BPTreeNode *child,
                            int *promoted_key, BPTreeNode **new_child_out) {
    BPTreeNode *new_node = bptree_create_node(false);
    int temp_keys[MAX_KEYS + 1];
    BPTreeNode *temp_children[MAX_CHILDREN + 1];

    for (int i = 0; i < MAX_KEYS; i++) temp_keys[i] = node->keys[i];
    for (int i = 0; i <= MAX_KEYS; i++) temp_children[i] = node->children[i];

    int pos = 0;
    while (pos < MAX_KEYS && key > temp_keys[pos]) pos++;

    for (int i = MAX_KEYS; i > pos; i--) {
        temp_keys[i] = temp_keys[i-1];
        temp_children[i+1] = temp_children[i];
    }
    temp_keys[pos] = key;
    temp_children[pos+1] = child;

    int split = (MAX_KEYS + 1) / 2;

    node->num_keys = 0;
    for (int i = 0; i < split; i++) {
        node->keys[i] = temp_keys[i];
        node->children[i] = temp_children[i];
        node->num_keys++;
    }
    node->children[split] = temp_children[split];

    *promoted_key = temp_keys[split];

    new_node->num_keys = 0;
    for (int i = split + 1, j = 0; i < MAX_KEYS + 1; i++, j++) {
        new_node->keys[j] = temp_keys[i];
        new_node->children[j] = temp_children[i];
        new_node->num_keys++;
    }
    new_node->children[new_node->num_keys] = temp_children[MAX_KEYS + 1];

    *new_child_out = new_node;
    return NULL;
}

// Protótipos privados (coloque no topo de ds.c)
BPTreeNode* bptree_delete_internal(BPTreeNode *node, int key, bool *deleted,
    BPTreeNode *parent, int child_index);

BPTreeNode* rebalance_after_delete(BPTreeNode *node);

// Função pública de remoção
BPTreeNode* bptree_delete(BPTreeNode *root, int key) {
    bool deleted = false;
    root = bptree_delete_internal(root, key, &deleted, NULL, -1);

    // se raiz ficou sem chave e não é folha, substitui por filho
    if (root && !root->is_leaf && root->num_keys == 0) {
        BPTreeNode *new_root = root->children[0];
        free(root);
        return new_root;
    }

    return root;
}

// Busca e remove a chave
BPTreeNode* bptree_delete_internal(BPTreeNode *node, int key, bool *deleted,
                                   BPTreeNode *parent, int child_index) {
    if (!node) return NULL;

    int pos = 0;
    while (pos < node->num_keys && node->keys[pos] < key) {
        pos++;
    }

    if (node->is_leaf) {
        // tenta encontrar e remover a chave na folha
        if (pos < node->num_keys && node->keys[pos] == key) {
            for (int i = pos; i < node->num_keys - 1; i++) {
                node->keys[i] = node->keys[i + 1];
                node->values[i] = node->values[i + 1];
            }
            node->num_keys--;
            *deleted = true;

            // verifica se precisa rebalancear
            int min_keys = (MAX_KEYS + 1) / 2;
            if (node->num_keys < min_keys && parent != NULL) {
                BPTreeNode *left_sibling = (child_index > 0) ? parent->children[child_index - 1] : NULL;
                BPTreeNode *right_sibling = (child_index < parent->num_keys) ? parent->children[child_index + 1] : NULL;

                // redistribuição com irmão à esquerda
                if (left_sibling && left_sibling->num_keys > min_keys) {
                    for (int i = node->num_keys; i > 0; i--) {
                        node->keys[i] = node->keys[i - 1];
                        node->values[i] = node->values[i - 1];
                    }
                    node->keys[0] = left_sibling->keys[left_sibling->num_keys - 1];
                    node->values[0] = left_sibling->values[left_sibling->num_keys - 1];
                    node->num_keys++;
                    left_sibling->num_keys--;

                    parent->keys[child_index - 1] = node->keys[0];
                    return node;
                }

                // redistribuição com irmão à direita
                if (right_sibling && right_sibling->num_keys > min_keys) {
                    node->keys[node->num_keys] = right_sibling->keys[0];
                    node->values[node->num_keys] = right_sibling->values[0];
                    node->num_keys++;

                    for (int i = 0; i < right_sibling->num_keys - 1; i++) {
                        right_sibling->keys[i] = right_sibling->keys[i + 1];
                        right_sibling->values[i] = right_sibling->values[i + 1];
                    }
                    right_sibling->num_keys--;

                    parent->keys[child_index] = right_sibling->keys[0];
                    return node;
                }

                // merge com irmão à esquerda
                if (left_sibling) {
                    for (int i = 0; i < node->num_keys; i++) {
                        left_sibling->keys[left_sibling->num_keys + i] = node->keys[i];
                        left_sibling->values[left_sibling->num_keys + i] = node->values[i];
                    }
                    left_sibling->num_keys += node->num_keys;
                    left_sibling->next = node->next;

                    // Remove chave do pai
                    for (int i = child_index - 1; i < parent->num_keys - 1; i++) {
                        parent->keys[i] = parent->keys[i + 1];
                        parent->children[i + 1] = parent->children[i + 2];
                    }
                    parent->num_keys--;

                    free(node);
                    return left_sibling;
                }

                // merge com irmão à direita
                if (right_sibling) {
                    for (int i = 0; i < right_sibling->num_keys; i++) {
                        node->keys[node->num_keys + i] = right_sibling->keys[i];
                        node->values[node->num_keys + i] = right_sibling->values[i];
                    }
                    node->num_keys += right_sibling->num_keys;
                    node->next = right_sibling->next;

                    // Remove chave do pai
                    for (int i = child_index; i < parent->num_keys - 1; i++) {
                        parent->keys[i] = parent->keys[i + 1];
                        parent->children[i + 1] = parent->children[i + 2];
                    }
                    parent->num_keys--;

                    free(right_sibling);
                    return node;
                }
            }

            return node;
        }

        // Chave não encontrada
        *deleted = false;
        return node;

    } else {
        // É nó interno → recursão
        node->children[pos] = bptree_delete_internal(node->children[pos], key, deleted, node, pos);

        // depois tentar rebalanceamento de nó interno
        return node;
    }
}



// e por fim uma função que irá nos mostrar o nó, por fins de debug
void bptree_print(BPTreeNode *node) {
    if (!node) return;

    if (node->is_leaf) {
        printf("[Leaf] ");
        for (int i = 0; i < node->num_keys; i++) {
            printf("%d ", node->keys[i]);
        }
        printf("-> ");
    } else {
        printf("[Internal] ");
        for (int i = 0; i < node->num_keys; i++) {
            printf("%d ", node->keys[i]);
        }
        printf("\n");
        for (int i = 0; i <= node->num_keys; i++) {
            bptree_print(node->children[i]);
        }
    }
}

void bptree_free(BPTreeNode *node) {
    if (!node) return;

    if (node->is_leaf) {
        for (int i = 0; i < node->num_keys; i++) {
            free(node->values[i]);  // libera a linha (array de void*)
        }
    }

    for (int i = 0; i <= node->num_keys; i++) {
        bptree_free(node->children[i]);
    }

    free(node);
}
