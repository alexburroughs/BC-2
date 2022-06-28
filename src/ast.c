#include "ast.h"
#include "arraylist.h"
#include "token.h"
#include "tree.h"
#include "panic.h"
#include "stack.h"
#include "stringbuilder.h"
#include "type.h"

#include <stdlib.h>
#include <stdbool.h>

#define UNEXPECTED(val, line, col) \
panic("Unexpected %s at line: %i:%i", String_from(val), line, col);

#define EXPECTED(val, line, col) \
panic("Expected %s at line: %i:%i", String_from(val), line, col);

#define COMPILER_PANIC(line, col) \
panic("Compiler panicked at %i:%i", line, col);

#define ADVANCE_TOKEN() \
++position; \
tk = (Token*)Arraylist_get(list, position);

#define REVERSE_TOKEN() \
--position; \
tk = (Token*)Arraylist_get(list, position);

#define IS_PROCESSING_ARG(state) \
(state == arg_state_Comma || state == arg_state_Iden || state == arg_state_Colon)

#define IS_OPERATOR(type) \
(type >= Add && type <= Divide) 

#define IS_BOOLEAN_OPERATOR(type) \
(type >= And && type <= Equal)

#define IS_BRACKET(type) \
(type >= OpenBracket && type <= CloseBracket)

#define IS_NUMBER_TYPE(type) \
(type <= F32_t)

int get_precedence(TokenType t, int line, int col)
{
    switch (t) {
        case Multiply:
        case Divide:
        //case Not:
            return 1;
        break;
        case Add:
        case Subtract:
            return 2;
        break;
        case Equal:
        case Greater:
        case GreaterEqual:
        case Less:
        case LessEqual:
        case NotEqual:
            return 3;
        break;
        case OpenBracket:
        case CloseBracket:
            return 4;
        break;
        default:
            UNEXPECTED("token in expression", line, col);
        break;
    }
}

ExpressionNode* parse_expr(Arraylist* list, int* pos) 
{
    int position = *pos;
    // For now, if a boolean operator is popped off the stack, a subsequent unary operator will cause an error
    bool is_boolean_expr = false;

    int control_flow_id = 0;

    ExpressionNode* expr = ExpressionNode_new();

    Stack* op_stack = (Stack*)Stack_new(free);
    Token* tk = (Token*)Arraylist_get(list, position);

    if (tk->type != OpenBracket) {
        UNEXPECTED("token", tk->line, tk->column)
    }

    Stack_push(op_stack, tk);

    ADVANCE_TOKEN()

    while (Arraylist_size(op_stack) > 0) {

        if (tk->type == Identifier) {
            Token* iden = tk;
            ADVANCE_TOKEN()

            if (tk->type != OpenBracket) {
                REVERSE_TOKEN()
                ExpressionNode_add(expr, GenericNode_new(VariableNode_t, VariableNode_new(String_from(tk->name), NULL, 0)));
            }
            else {
                ADVANCE_TOKEN()
                CallNode* cn = CallNode_new(String_from(iden->name));
                while (tk->type != CloseBracket) {
                    GenericNode* cn_arg;
                    VariableNode* call_vn;
                    LiteralNode* call_ln;
                    switch(tk->type) {
                        case Identifier:
                            call_vn = VariableNode_new(String_from(tk->name), NULL, 0);
                            cn_arg = GenericNode_new(VariableNode_t, call_vn);
                        break;
                        case Number:
                            call_ln = LiteralNode_new(String_from(tk->name), I32_t);
                            cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                        break;
                        case String:
                            call_ln = LiteralNode_new(String_from(tk->name), Char_t);
                            cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                        break;
                        case Comma:
                            ADVANCE_TOKEN()
                            continue;
                        break;
                    }
                    CallNode_add_arg(cn, cn_arg);
                    ADVANCE_TOKEN()
                }
                ExpressionNode_add(expr, GenericNode_new(CallNode_t, cn));
            }
            ADVANCE_TOKEN()
        }
        else if (tk->type == OpenBracket) {
            Stack_push(op_stack, tk);
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

            ExpressionNode_add(expr, GenericNode_new(LiteralNode_t, LiteralNode_new(StringBuilder_get(sb), I32_t)));
            StringBuilder_free(sb);
        }
        else if (IS_OPERATOR(tk->type) || IS_BOOLEAN_OPERATOR(tk->type) || IS_BRACKET(tk->type)) {

            while (get_precedence(tk->type, tk->line, tk->column) 
                    > get_precedence(((Token*)Stack_peek(op_stack))->type, tk->line, tk->column) 
                    && ((Token*)Stack_peek(op_stack))->type != OpenBracket) {
            
                Token* operator = Stack_pop(op_stack);

                if (is_boolean_expr && !IS_BOOLEAN_OPERATOR(operator->type))
                    UNEXPECTED("operator, cannot perform operation on boolean expression", operator->line, operator->column);

                if (!is_boolean_expr && IS_BOOLEAN_OPERATOR(operator->type))
                    is_boolean_expr = true;

                ExpressionNode_add(expr, GenericNode_new(OperatorNode_t, OperatorNode_new(from_token(operator->type))));
            }
            
            if (tk->type == CloseBracket)
                Stack_pop(op_stack);
            else {
                Stack_push(op_stack, tk);
            }
            
            if (IS_BOOLEAN_OPERATOR(tk->type))
                is_boolean_expr = true;

            ADVANCE_TOKEN()
            
        }
        else {
            UNEXPECTED("token", tk->line, tk->column)
        }

    }

    *pos = position;
    return expr;
}

AST* AST_new()
{
    AST* ast = (AST*)malloc(sizeof(AST));

    ast->functions = Hashmap_new(GenericNode_free);
    ast->imports = Arraylist_new(free);
    return ast;
}

void AST_free(AST* ast)
{
    Hashmap_free(ast->functions);
    Arraylist_free(ast->imports);
}

AST* AST_from(Arraylist* list)
{
    AST* ast = AST_new();
    State state = Begin;
    int id = 0;

    int position = 0;

    Token* last;
    Token* tk = NULL;
    
    StringBuilder* sb = StringBuilder_new();
    bool in_func = false;
    Stack* scope = Stack_new(GenericNode_free);
    tk = (Token*)Arraylist_get(list, position);
    while(tk->type == Import) {
        ADVANCE_TOKEN()
        Arraylist_add(ast->imports, String_from(tk->name));
        ADVANCE_TOKEN()
    }

    while(position < Arraylist_size(list)) {
        last = tk;
        tk = (Token*)Arraylist_get(list, position);

        switch(tk->type) {
            case Function:
                // No lambdas for now
                if (in_func)
                    UNEXPECTED("function declaration", tk->line, tk->column)
                in_func = true;

                // Get function name 
                ADVANCE_TOKEN()
                if (tk->type != Identifier)
                    UNEXPECTED("token", tk->line, tk->column)
                if (tk->name == NULL)
                    COMPILER_PANIC(tk->line, tk->column)
                FunctionNode* fn = FunctionNode_new(String_from(tk->name));
                GenericNode* gn = GenericNode_new(FunctionNode_t, fn);
                
                Hashmap_insert(ast->functions, String_from(tk->name), gn);
                Stack_push(scope, gn);
                in_func = true;

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
                Type type;

                // Loop until all arguments are processed
                while (tk->type != CloseBracket) {
                    switch (a_state)
                    {
                    case arg_state_Start:
                        if (tk->type != Identifier)
                            UNEXPECTED("token", tk->line, tk->column)
                        name = String_from(tk->name);
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
                        type = 0;
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
                            type = I8_t;
                            break;
                        case I16:
                            type = I16_t;
                            break;
                        case I32:
                            type = I32_t;
                            break;
                        case I64:
                            type = I64_t;
                            break;
                        case U8:
                            type = U8_t;
                            break;
                        case U16:
                            type = U16_t;
                            break;
                        case U32:
                            type = U32_t;
                            break;
                        case U64:
                            type = U64_t;
                            break;
                        case Char:
                            type = Char_t;
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
                if (a_state == arg_state_Type) {
                    current_vn = VariableNode_new(String_from(name), NULL, type);
                    FunctionNode_add_arg(fn, current_vn);
                }
                ADVANCE_TOKEN()

                // Get return type if there is one
                if (tk->type == Colon) {
                    ADVANCE_TOKEN()

                    switch (tk->type)
                    {
                    case I8:
                        type = I8_t;
                        break;
                    case I16:
                        type = I16_t;
                        break;
                    case I32:
                        type = I32_t;
                        break;
                    case I64:
                        type = I64_t;
                        break;
                    case U8:
                        type = U8_t;
                        break;
                    case U16:
                        type = U16_t;
                        break;
                    case U32:
                        type = U32_t;
                        break;
                    case U64:
                        type = U64_t;
                        break;
                    case Char:
                        type = Char_t;
                        break;
                    case Void:
                        type = Void_t;
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

                ADVANCE_TOKEN()
                break;
            case If:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)

                ADVANCE_TOKEN()
                
                IfNode* in = IfNode_new(String_from_int(id++), parse_expr(list, &position));
                
                tk = (Token*)Arraylist_get(list, position);
                if (tk->type != OpenBrace)
                    UNEXPECTED("Missing open bracket in if statement", tk->line, tk->column)
                GenericNode* generic_in = GenericNode_new(IfNode_t, in);
                GenericNode_add_statement((GenericNode*)Stack_peek(scope), generic_in);
                Stack_push(scope, generic_in);

                break;
            case While:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)

                ADVANCE_TOKEN()

                WhileNode* wn = IfNode_new(String_from_int(id++), parse_expr(list, &position));
                
                tk = (Token*)Arraylist_get(list, position);

                if (tk->type != OpenBrace)
                    UNEXPECTED("Missing open bracket in while statement", tk->line, tk->column)
                GenericNode* generic_wn = GenericNode_new(WhileNode_t, wn);
                GenericNode_add_statement((GenericNode*)Stack_peek(scope), generic_wn);
                Stack_push(scope, generic_wn);

                break;
            case Else:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                
                ElseNode* en = ElseNode_new(String_from_int(id++));

                ADVANCE_TOKEN()

                if (tk->type != OpenBrace)
                    UNEXPECTED("Missing open bracket in else statement", tk->line, tk->column)

                if (GenericNode_get_last_node_type((GenericNode*)Stack_peek(scope)) != IfNode_t)
                    UNEXPECTED("Else must have matching if statement", tk->line, tk->column)
                
                GenericNode* generic_en = GenericNode_new(ElseNode_t, en);
                GenericNode_add_statement((GenericNode*)Stack_peek(scope), generic_en);
                Stack_push(scope, generic_en);
                break;
            case CloseBrace:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                if (((GenericNode*)Stack_pop(scope))->type == FunctionNode_t)
                    in_func = false;
                break;
            case Variable: 
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                ADVANCE_TOKEN()

                if (tk->type != Identifier)
                    UNEXPECTED("token", tk->line, tk->column)

                char* variable_name = tk->name;
                Type variable_type;

                ADVANCE_TOKEN()
                if (tk->type != Colon)
                    UNEXPECTED("token", tk->line, tk->column)
                ADVANCE_TOKEN()

                switch (tk->type) {
                    case I8:
                        variable_type = I8_t;
                        break;
                    case I16:
                        variable_type = I16_t;
                        break;
                    case I32:
                        variable_type = I32_t;
                        break;
                    case I64:
                        variable_type = I64_t;
                        break;
                    case U8:
                        variable_type = U8_t;
                        break;
                    case U16:
                        variable_type = U16_t;
                        break;
                    case U32:
                        variable_type = U32_t;
                        break;
                    case U64:
                        variable_type = U64_t;
                        break;
                    case Char:
                        variable_type = Char_t;
                        break;
                    case F32:
                        variable_type = F32_t;
                        break;
                    case F64:
                        variable_type = F64_t;
                        break;
                    default:
                        UNEXPECTED("type", tk->line, tk->column)
                        break;
                }

                DeclarationNode* dn =  DeclarationNode_new(String_from(variable_name), variable_type);
                GenericNode_add_statement((GenericNode*)Stack_peek(scope), GenericNode_new(DeclarationNode_t, dn));
                ADVANCE_TOKEN()

                if (tk->type == Assign) {
                    AssignmentNode* an;
                    if (IS_NUMBER_TYPE(variable_type)) {
                        ADVANCE_TOKEN()
                        an = AssignmentNode_new(String_from(variable_name), parse_expr(list, &position));
                        tk = (Token*)Arraylist_get(list, position);
                    }
                    else {
                        ADVANCE_TOKEN()
                        ExpressionNode* en = ExpressionNode_new();
                        switch (tk->type) {
                            case String:
                                ExpressionNode_add(en, GenericNode_new(LiteralNode_t, LiteralNode_new(String_from(tk->name), Char_t)));
                            break;
                            case Identifier:
                                
                                if (((Token*)Arraylist_get(list, position+1))->type == OpenBracket) {
                                    
                                    CallNode* cn = CallNode_new(String_from(tk->name));
                                    ADVANCE_TOKEN()
                                    ADVANCE_TOKEN()
                                    while (tk->type != CloseBracket) {
                                        GenericNode* cn_arg;
                                        VariableNode* call_vn;
                                        LiteralNode* call_ln;
                                        switch(tk->type) {
                                            case Identifier:
                                                call_vn = VariableNode_new(String_from(tk->name), NULL, 0);
                                                cn_arg = GenericNode_new(VariableNode_t, call_vn);
                                            break;
                                            case Number:
                                                call_ln = LiteralNode_new(String_from(tk->name), I32_t);
                                                cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                                            break;
                                            case String:
                                                call_ln = LiteralNode_new(String_from(tk->name), Char_t);
                                                cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                                            break;
                                            case Comma:
                                                ADVANCE_TOKEN()
                                                continue;
                                            break;
                                        }
                                        CallNode_add_arg(cn, cn_arg);
                                        ADVANCE_TOKEN()
                                    }
                                    ExpressionNode_add(en, GenericNode_new(CallNode_t, cn));
                                }
                                else {
                                    ExpressionNode_add(en, GenericNode_new(VariableNode_t, VariableNode_new(String_from(tk->name), NULL, 0)));
                                }
                            break;
                        }
                        an = AssignmentNode_new(String_from(variable_name), en);
                        ADVANCE_TOKEN()

                    }
                    GenericNode_add_statement((GenericNode*)Stack_peek(scope), GenericNode_new(AssignmentNode_t, an));
                }
                
                REVERSE_TOKEN()
                break;
            case Identifier:
                if (!in_func)
                    UNEXPECTED("Statement must be in a function", tk->line, tk->column)
                
                char* identifier_name = tk->name;
                ADVANCE_TOKEN()
                
                    
                if (tk->type == Assign) {
                    AssignmentNode* an;
                    ADVANCE_TOKEN()
                    if (tk->type == OpenBracket) {
                        an = AssignmentNode_new(String_from(identifier_name), parse_expr(list, &position));
                        tk = (Token*)Arraylist_get(list, position);
                    }
                    else {
                        ExpressionNode* en = ExpressionNode_new();
                        switch (tk->type) {
                            case String:
                                ExpressionNode_add(en, GenericNode_new(LiteralNode_t, LiteralNode_new(String_from(tk->name), Char_t)));
                            break;
                            case Identifier:
                                if (((Token*)Arraylist_get(list, position+1))->type == OpenBracket) {
                                    CallNode* cn = CallNode_new(String_from(tk->name));
                                    ADVANCE_TOKEN()
                                    ADVANCE_TOKEN()
                                    while (tk->type != CloseBracket) {
                                        GenericNode* cn_arg;
                                        VariableNode* call_vn;
                                        LiteralNode* call_ln;
                                        switch(tk->type) {
                                            case Identifier:
                                                call_vn = VariableNode_new(String_from(tk->name), NULL, 0);
                                                cn_arg = GenericNode_new(VariableNode_t, call_vn);
                                            break;
                                            case Number:
                                                call_ln = LiteralNode_new(String_from(tk->name), I32_t);
                                                cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                                            break;
                                            case String:
                                                call_ln = LiteralNode_new(String_from(tk->name), Char_t);
                                                cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                                            break;
                                            case Comma:
                                                ADVANCE_TOKEN()
                                                continue;
                                            break;
                                        }
                                        CallNode_add_arg(cn, cn_arg);
                                        ADVANCE_TOKEN()
                                    }
                                    ExpressionNode_add(en, GenericNode_new(CallNode_t, cn));
                                }
                                else {
                                    ExpressionNode_add(en, GenericNode_new(VariableNode_t, VariableNode_new(String_from(tk->name), NULL, 0)));
                                }
                            break;
                        }
                        an = AssignmentNode_new(String_from(identifier_name), en);
                        
                    }
                    GenericNode_add_statement((GenericNode*)Stack_peek(scope), GenericNode_new(AssignmentNode_t, an));
                }
                else if (tk->type == OpenBracket) {
                    ADVANCE_TOKEN()
                    CallNode* cn = CallNode_new(String_from(identifier_name));
                    while (tk->type != CloseBracket) {
                        GenericNode* cn_arg;
                        LiteralNode* call_ln;
                        VariableNode* call_vn;
                        switch(tk->type) {
                            case Identifier:
                                call_vn = VariableNode_new(String_from(tk->name), NULL, 0);
                                cn_arg = GenericNode_new(VariableNode_t, call_vn);
                            break;
                            case Number:
                                call_ln = LiteralNode_new(String_from(tk->name), I32_t);
                                cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                            break;
                            case String:
                                call_ln = LiteralNode_new(String_from(tk->name), Char_t);
                                cn_arg = GenericNode_new(LiteralNode_t, call_ln);
                            break;
                            case Comma:
                                ADVANCE_TOKEN()
                                continue;
                            break;
                        }
                        CallNode_add_arg(cn, cn_arg);
                        ADVANCE_TOKEN()
                    }
                    GenericNode_add_statement((GenericNode*)Stack_peek(scope), GenericNode_new(CallNode_t, cn));
                }
                else {
                    ReturnNode* rn = ReturnNode_new(String_from(identifier_name));
                    GenericNode_add_statement((GenericNode*)Stack_peek(scope), GenericNode_new(ReturnNode_t, rn));
                    REVERSE_TOKEN()
                }
                
                break;
            default:
                
                //panic("Unexpected Token %i, pos:%i", tk->type, position);
                UNEXPECTED("token at top", tk->line, tk->column);
                break;
        }
        ADVANCE_TOKEN()
    }

    return ast;
}