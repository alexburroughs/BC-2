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

#define EXPECTED(val, line, col) \
panic("Expected %s at line: %i:%i", val, line, col);

#define COMPILER_PANIC(line, col) \
panic("Compiler panicked at %i:%i", line, col);

#define ADVANCE_TOKEN() \
++position; \
tk = (Token*)Arraylist_get(list, position);

#define IS_PROCESSING_ARG(state) \
(state == arg_state_Comma || state == arg_state_Iden || state == arg_state_Colon)

#define IS_OPERATOR(type) \
(type >= Add && type <= Divide) 

#define IS_BINARY_OPERATOR(type) \
(type >= And && type <= Equal)

int get_precedence(TokenType t, int line, int col)
{
    switch (t) {

        case Add:
        case Subtract:
            return 1;
        break;
        case Multiply:
        case Divide:
            return 2;
        break;
        default:
            UNEXPECTED("token in expression", line, col);
        break;
    }
}

ExpressionNode* parse_expr(Arraylist* list, int position) 
{
    ExpressionNode* expr = ExpressionNode_new();

    Stack* scope_stack = (Stack*)Stack_new(free);
    Token* tk = (Token*)Arraylist_get(list, position);

    if (tk->type != OpenBrace)
        UNEXPECTED("token", tk->line, tk->column)

    Stack_push(scope_stack, tk);

    ADVANCE_TOKEN()

    while (Arraylist_size(scope_stack) > 0) {

        if (tk->type == Identifier) {
            Token* iden = tk;
    
            ADVANCE_TOKEN()

            if (tk->type != OpenBracket) {
                ExpressionNode_add(expr, GenericNode_new(VariableNode_t, VariableNode_new(tk->name, NULL, NULL)));
            }
        }

        else if (IS_OPERATOR(tk->type)) {
            ADVANCE_TOKEN()
        }

        else if (IS_BINARY_OPERATOR(tk->type)) {
            ADVANCE_TOKEN()
        }

        else if (tk->type == Number) {

            StringBuilder* sb = StringBuilder_new();

            StringBuilder_add_arr(sb, tk->name);
            
            ADVANCE_TOKEN() 
            
            if (tk->type == Dot) {

                StringBuilder_add(sb, '.');
                ADVANCE_TOKEN()
            
                if (tk->type != Number)
                    UNEXPECTED("token", tk->line, tk->column)
            
                StringBuilder_add_arr(sb, tk->name);
                ADVANCE_TOKEN()
            }

            ExpressionNode_add(expr, GenericNode_new(LiteralNode_t, LiteralNode_new(StringBuilder_get(sb))));
        }

   }

    return expr;
}

AST* AST_new()
{
    AST* ast = (AST*)malloc(sizeof(AST));

    ast->functions = Hashmap_new(GenericNode_free);
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
        Stack* scope = Stack_new(GenericNode_free);
        last = tk;
        tk = (Token*)Arraylist_get(list, position);

        switch(tk->type) {
            case Function:
                // No lambdas for now
                if (in_func)
                    UNEXPECTED("function declaration", tk->line, tk->column)
                in_func = true;

                ADVANCE_TOKEN()
                if (tk->type != Identifier)
                    UNEXPECTED("token", tk->line, tk->column)
                if (tk->name == NULL)
                    COMPILER_PANIC(tk->line, tk->column)
                FunctionNode* fn = FunctionNode_new(tk->name);
                GenericNode* gn = GenericNode_new(FunctionNode_t, fn);

                Hashmap_insert(ast->functions, String_from(tk->name), gn);
                Stack_push(scope, gn);

                in_func = true;
                tk->name = NULL;

                ADVANCE_TOKEN()

                if (tk->type != OpenBracket)
                    UNEXPECTED("token", tk->line, tk->column);
                
                ADVANCE_TOKEN()

                enum arg_state {
                    arg_state_Start,
                    arg_state_Comma,
                    arg_state_Iden,
                    arg_state_Type,
                    arg_state_Colon
                };

                enum arg_state a_state = arg_state_Start;

                VariableNode* current_vn;
                char* name;
                char* type;

                while (tk->type != CloseBracket) {
                    switch (a_state)
                    {
                    case arg_state_Start:
                        if (tk->type != Identifier)
                            UNEXPECTED("token", tk->line, tk->column)
                        name = String_from(name);
                        a_state = arg_state_Iden;
                        break;
                    case arg_state_Iden:
                        if (tk->type != Colon)
                            UNEXPECTED("token", tk->line, tk->column)
                        a_state = arg_state_Colon;
                        break;
                    case arg_state_Type:
                        if (tk->type != Comma)
                            UNEXPECTED("token", tk->line, tk->column)
                        a_state = arg_state_Comma;
                        break;
                    case arg_state_Comma:
                        current_vn = VariableNode_new(name, NULL, type);

                        FunctionNode_add_arg(fn, current_vn);
                        name = NULL;
                        type = NULL;
                        current_vn = NULL;
                        
                        if (tk->type != Identifier)
                            UNEXPECTED("token", tk->line, tk->column)
                        name = String_from(name);
                        a_state = arg_state_Iden;
                        break;
                    case arg_state_Colon:
                        switch (tk->type)
                        {
                        case I8:
                            type = String_from("I8");
                            break;
                        case I16:
                            type = String_from("I16");
                            break;
                        case I32:
                            type = String_from("I32");
                            break;
                        case I64:
                            type = String_from("I64");
                            break;
                        case U8:
                            type = String_from("U8");
                            break;
                        case U16:
                            type = String_from("U16");
                            break;
                        case U32:
                            type = String_from("U32");
                            break;
                        case U64:
                            type = String_from("U64");
                            break;
                        case Char:
                            type = String_from("Char");
                            break;
                        default:
                            UNEXPECTED("type", tk->line, tk->column)
                            break;
                        }
                        a_state = arg_state_Type;
                        break;
                    default:
                        break;
                    }

                    ADVANCE_TOKEN()
                }

                if (IS_PROCESSING_ARG(a_state))
                    EXPECTED("argument", tk->line, tk->column);
                ADVANCE_TOKEN()

                if (tk->type == Colon) {
                    ADVANCE_TOKEN()

                    switch (tk->type)
                    {
                    case I8:
                        type = String_from("I8");
                        break;
                    case I16:
                        type = String_from("I16");
                        break;
                    case I32:
                        type = String_from("I32");
                        break;
                    case I64:
                        type = String_from("I64");
                        break;
                    case U8:
                        type = String_from("U8");
                        break;
                    case U16:
                        type = String_from("U16");
                        break;
                    case U32:
                        type = String_from("U32");
                        break;
                    case U64:
                        type = String_from("U64");
                        break;
                    case Char:
                        type = String_from("Char");
                        break;
                    default:
                        UNEXPECTED("type", tk->line, tk->column)
                        break;
                    }

                    FunctionNode_set_return(fn, type);
                }
                else if (tk->type != OpenBrace) {
                    UNEXPECTED("token", tk->line, tk->column)
                }
                break;
            case If:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)

                ADVANCE_TOKEN()

                IfNode* in = IfNode_new("", parse_expr(list, position));

                break;
            case While:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                break;
            case Else:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                break;
            case CloseBrace: 
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                break;
            case Variable: 
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                break;
            default:
                panic("Unexpected Token %s");
                break;
        }
    }
}