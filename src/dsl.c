#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include "../include/dsl.h"

// Agora criaremos a nossa linguagem estruturada de consulta, como sql
// Vamos iniciar com comandos de SELECT, INSERT, DELETE  e CREATE
// vamos fazer o parser e depois as funções para fazer as execuções

// Hash de tabelas criadas
#define MAX_TABLES 32
Table *tables[MAX_TABLES];
int table_count = 0;

// Função auxiliar para transformar string em ColumnType
ColumnType str_to_type(const char *type_str) {
    if (strcmp(type_str, "int") == 0) return COL_INT;
    if (strcmp(type_str, "string") == 0) return COL_STRING;
    if (strcmp(type_str, "float") == 0) return COL_FLOAT;
    return -1;
}

// Função principal que roda o interpretador
void dsl_repl() {
    char *line;
    printf("Welcome to GrayDB:\n");
    while ((line = readline(">> ")) != NULL) {
        if (strcmp(line, "exit") == 0) {
            free(line);
            break;
        }
        parse_and_execute(line);
        free(line);
    }
}

// Função que interpreta e executa o comando
void parse_and_execute(const char *command) {
    char cmd[512];
    strncpy(cmd, command, 511);
    cmd[511] = '\0';

    // CREATE TABLE nome (col tipo, e etc)
    if (strncasecmp(cmd, "create table", 12) == 0) {
        char *tok = strtok(cmd + 13, " (");
        char *table_name = tok;

        Column columns[16];
        int num_cols = 0;

        while ((tok = strtok(NULL, ","))) {
            char *colname = strtok(tok, " ");
            char *coltype = strtok(NULL, " \n)");
            if (!colname || !coltype) break;

            columns[num_cols].name = strdup(colname);
            columns[num_cols].type = str_to_type(coltype);
            num_cols++;
        }

        if (table_count < MAX_TABLES) {
            tables[table_count++] = table_create(table_name, columns, num_cols);
            printf("Table '%s' created with %d columns.\n", table_name, num_cols);
        }

    // INSERT INTO nome VALUES
    } else if (strncasecmp(cmd, "insert into", 11) == 0) {
        char *tok = strtok(cmd + 12, " ");
        char *table_name = tok;

        tok = strtok(NULL, " ()");
        if (strcmp(tok, "values") != 0) {
            printf("Sintax error\n");
            return;
        }

        Table *t = NULL;
        for (int i = 0; i < table_count; i++) {
            if (strcmp(tables[i]->table_name, table_name) == 0) {
                t = tables[i];
                break;
            }
        }

        if (!t) {
            printf("Table not found.\n");
            return;
        }

        void **row = malloc(sizeof(void*) * t->num_columns);
        for (int i = 0; i < t->num_columns; i++) {
            char *val = strtok(NULL, ",)");
            if (!val) break;

            while (isspace(*val)) val++; // ignora espaços

            switch (t->columns[i].type) {
                case COL_INT:
                    row[i] = malloc(sizeof(int));
                    *((int*)row[i]) = atoi(val);
                    break;
                case COL_FLOAT:
                    row[i] = malloc(sizeof(float));
                    *((float*)row[i]) = atof(val);
                    break;
                case COL_STRING:
                    row[i] = strdup(val);
                    break;
            }
        }

        table_insert(t, row);
        printf("Inserted in table with success %s.\n", table_name);

    // SELECT * FROM nome WHERE id = val
    } else if (strncasecmp(cmd, "select", 6) == 0) {
        char *table_name = strtok(cmd + 14, " ");
        strtok(NULL, "=");  // ignora "where id"
        char *val = strtok(NULL, " \n");

        int id = atoi(val);

        Table *t = NULL;
        for (int i = 0; i < table_count; i++) {
            if (strcmp(tables[i]->table_name, table_name) == 0) {
                t = tables[i];
                break;
            }
        }

        if (!t) {
            printf("Table not found.\n");
            return;
        }

        void **res = table_search(t, id);
        if (res) {
            printf("Result:\n");
            for (int i = 0; i < t->num_columns; i++) {
                Column *col = &t->columns[i];
                printf("%s: ", col->name);
                if (col->type == COL_INT)    printf("%d\n", *((int*)res[i]));
                if (col->type == COL_FLOAT)  printf("%.2f\n", *((float*)res[i]));
                if (col->type == COL_STRING) printf("%s\n", (char*)res[i]);
            }
        } else {
            printf("Register not found.\n");
        }
    }
}
