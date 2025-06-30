#ifndef DSL_H
#define DSL_H

#include "table.h"

// DSL init
void dsl_repl();  // inicia o interpretador (modo interativo)

// Utilidades de parsing
void parse_and_execute(const char *command);

#endif
