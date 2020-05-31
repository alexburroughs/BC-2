#include "ast.h"
#include "arraylist.h"
#include "token.h"
#include "tree.h"
#include "panic.h"
#include "stack.h"

#include <stdlib.h>

AST* AST_new()
{
    AST* ast = (AST*)malloc(sizeof(AST));

    ast->functions = Hashmap_new(free);
}

AST* AST_from(Arraylist* list)
{
    AST* ast = AST_new();
    State state = Begin;

    int position = 0;

    Token* last;
    Token* tk = NULL;

    while(position < Arraylist_size(list)) {

        last = tk;
        tk = (Token*)Arraylist_get(list, position);

        switch(tk->type) {
            case Function:
                break;
            case If:
                break;
            case While:
                break;
            case Else:
                break;
            case CloseBrace:
                break;
            case Variable:
                break;
            default:
                panic("Unexpected Token %s");
                break;
        }
    }
}