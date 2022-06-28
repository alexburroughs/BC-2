#ifndef NNIGENERATOR_H
#define NNIGENERATOR_H
#include "ast.h"

typedef struct variable {
    int position;
    Type type;
} VariableObj;

VariableObj* Variable_new(int position, Type type);

char* ast_to_nni(AST* ast);
#endif