#include "ast.h"
#include "arraylist.h"
#include "token.h"
#include "tree.h"
#include "panic.h"
#include "stack.h"
#include "stringbuilder.h"

#include <stdlib.h>
#include <stdbool.h>

#define UNEXPECTED(val, line, col) \
panic("Unexpected %s at line: %i:%i", val, line, col);

#define ADVANCE_TOKEN \
++position; \
tk = (Token*)Arraylist_get(list, position);

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

        StringBuilder* sb = StringBuilder_new();
        bool in_func = false;
        char* scope = NULL;
        last = tk;
        tk = (Token*)Arraylist_get(list, position);

        switch(tk->type) {
            case Function:
                if (in_func)
                    UNEXPECTED("function declaration", tk->line, tk->column)
                    //FunctionNode* fn = FunctionNode_new();
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