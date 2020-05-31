#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Token* Token_new(TokenType type, char* name, int line, int column) 
{
    Token* t = malloc(sizeof(Token));
    t->type = type;
    t->line = line;
    t->column = column;
    t->name = name;
}

void Token_free(Token* tk)
{
    free(tk->name);
    free(tk);
}