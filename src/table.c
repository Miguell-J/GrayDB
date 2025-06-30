#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/table.h"
#include "../include/bptree.h"

BPTreeNode* bptree_delete(BPTreeNode *root, int key);

// Cria a tabela, aloca colunas e inicializa B+ tree
Table* table_create(const char *table_name, Column *columns, int num_columns) {
    Table *table = malloc(sizeof(Table));
    if (!table) return NULL;

    table->table_name = strdup(table_name);
    table->num_columns = num_columns;
    table->columns = malloc(sizeof(Column) * num_columns);

    for (int i = 0; i < num_columns; i++) {
        table->columns[i].name = strdup(columns[i].name);
        table->columns[i].type = columns[i].type;
    }

    table->index_root = NULL;  // começa vazia

    return table;
}

// Insere uma linha usando a chave primária da primeira coluna
bool table_insert(Table *table, void **values) {
    if (!table || !values) return false;

    int primary_key;

    // verifica se a primeira coluna é INT, pra pegar a chave primária
    if (table->columns[0].type != COL_INT) {
        printf("The primary key must be of type INT\n");
        return false;
    }

    primary_key = *((int*)values[0]);

    // Insere na B+ Tree (index_root) a chave e os valores da linha
    table->index_root = bptree_insert(table->index_root, primary_key, values);

    return true;
}

// Busca pela chave primária (int)
void** table_search(Table *table, int primary_key) {
    if (!table) return NULL;

    void *res = bptreesearch(table->index_root, primary_key);
    return (void**)res;  // cast para array de void* (linha)
}

// Libera memória da tabela
void table_free(Table *table) {
    if (!table) return;

    for (int i = 0; i < table->num_columns; i++) {
        free(table->columns[i].name);
    }
    free(table->columns);

    // aqui nos não liberamos as linhas porque estão dentro da B+ Tree e dependem do gerenciamento do banco

    free(table->table_name);
    free(table);
}

bool table_delete(Table *table, int primary_key) {
    if (!table) return false;

    table->index_root = bptree_delete(table->index_root, primary_key);
    return true;
}


void table_print(Table *table) {
    if (!table) return;
    printf("Table: %s\n", table->table_name);
    printf("Columns: ");
    for (int i = 0; i < table->num_columns; i++) {
        printf("%s ", table->columns[i].name);
    }
    printf("\n");
    bptree_print(table->index_root); 
}


