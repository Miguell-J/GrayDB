#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h>
#include "bptree.h"  // a B+ tree já feita


// primeiro criamos um enum contendo os tipo de dados comportados nas colunas, adicionarei mais depois
// agora iremos comportar inteiros, strings, e float
typedef enum {
    COL_INT,
    COL_STRING,
    COL_FLOAT,
    // você pode adicionar mais tipos depois
} ColumnType;

// aqui criamos a struct da coluna, recebemos um nome para a coluna, e então um ponteiro para o tipo de dado da coluna
typedef struct {
    char *name;
    ColumnType type;
} Column;


// agora que podemos criar tabelas, criamos o struct para a tabela
// recebemos um nome para a tabela, dai o numero de colunas e então um ponteiro para o struct das colunas
// logo depois recebmos um ponteiro para a arvore bptree, para fazermos a indexação por chave primaria
typedef struct {
    char *table_name;

    int num_columns;
    Column *columns;

    BPTreeNode *index_root; // raiz da B+ Tree para indexar por chave primária

} Table;

// criamos uma função para uma nova tabela com as colunas definidas
Table* table_create(const char *table_name, Column *columns, int num_columns);

// e agora com a tabela criada, criamos uma função para:
// Insere uma linha na tabela.
// values é array de ponteiros para os dados, seguindo a ordem das colunas.
// A primeira coluna será usada como chave primária na B+ Tree.
bool table_insert(Table *table, void **values);


// Busca uma linha pela chave primária (int)
void** table_search(Table *table, int primary_key);

// Função para liberar a memória da tabela (incluindo colunas)
void table_free(Table *table);

#endif
