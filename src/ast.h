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
};

typedef struct ast AST;
typedef enum state State;

AST* AST_from(Arraylist* arr);

#endif