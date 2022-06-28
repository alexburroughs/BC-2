#ifndef AST_H
#define AST_H

#include "tree.h"

enum state
{
    Begin
};

struct ast
{
    Hashmap* functions;
    Arraylist* imports;
};

typedef struct ast AST;
typedef enum state State;

AST* AST_from(Arraylist* arr);
void AST_free(AST* ast);
//debug
ExpressionNode* parse_expr(Arraylist* list, int* position);

#endif