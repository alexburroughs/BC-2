#include "ast.h"
#include "hashmap.h"
#include "tree.h"
#include "stringbuilder.h"
#include "stack.h"
#include "panic.h"
#include "nonamegenerator.h"
#include "type.h"

#include <stdlib.h>
#include <stdbool.h>
#include <uuid/uuid.h>

#ifndef UUID_STR_LEN
#define UUID_STR_LEN 37
#endif

#define IS_NUMBER_TYPE(x)\
(strcmp(x, "I32")|| \
strcmp(x, "U32"))

char* get_uuid()
{
    uuid_t binuuid;
    uuid_generate_random(binuuid);
    char *uuid = malloc(UUID_STR_LEN);
    uuid_unparse(binuuid, uuid);

    return uuid;
}

VariableObj* Variable_new(int position, Type type)
{
    VariableObj* variable = malloc(sizeof(VariableObj));
    variable->position = position;
    variable->type = type;
}

void assign_literal(StringBuilder* code_sb, LiteralNode* ln, int position)
{
    StringBuilder_add_arr(code_sb, "SET ");
    StringBuilder_add_arr(code_sb, String_from_int(position));

    switch(ln->type) {
        case Char_t:
            StringBuilder_add_arr(code_sb, " STR ");
            StringBuilder_add_arr(code_sb, "\"");
            StringBuilder_add_arr(code_sb, String_from(ln->name));
            StringBuilder_add_arr(code_sb, "\"");
        break;
        default:
            StringBuilder_add_arr(code_sb, " NUM ");
            StringBuilder_add_arr(code_sb, String_from(ln->name));
        break;
    }
    
    StringBuilder_add_arr(code_sb, "\n");
}

int make_literal_variable(StringBuilder* code_sb, Hashmap* variable_map, int* variable_tos, LiteralNode* literal)
{
    int index = *variable_tos;
    (*variable_tos)++;
    Hashmap_insert(variable_map, String_from_int(index), Variable_new(index, literal->type));
    parse_declaration(DeclarationNode_new(String_from_int(index), literal->type), code_sb);
    assign_literal(code_sb, literal, index);
    return index;
}

void parse_call(StringBuilder* code_sb, Hashmap* variable_map, int* variable_tos, CallNode* cn)
{
    StringBuilder* call_sb = StringBuilder_new();
    StringBuilder_add_arr(call_sb, String_from("CALL "));
    StringBuilder_add_arr(call_sb, String_from(cn->name));
    for (int call_arg = 0; call_arg < Arraylist_size(cn->args); call_arg++) {
        GenericNode* gn = (GenericNode*)Arraylist_get(cn->args, call_arg);
        StringBuilder_add_arr(call_sb, String_from(" "));
        LiteralNode* call_ln;
        VariableNode* call_vn;
        
        switch (gn->type) {
            case LiteralNode_t:
                call_ln = (LiteralNode*)gn->node;
                StringBuilder_add_arr(call_sb, String_from_int(make_literal_variable(code_sb, variable_map, variable_tos, call_ln)));      
            break;
            case VariableNode_t:
                call_vn = (VariableNode*)gn->node;
                StringBuilder_add_arr(call_sb, String_from_int(((VariableObj*)Hashmap_get(variable_map, call_vn->name))->position));
            break;
        }
    }

    StringBuilder_add_arr(call_sb, "\n");
    StringBuilder_add_arr(code_sb, StringBuilder_get(call_sb));
    StringBuilder_free(call_sb);
}

void parse_expression(Hashmap* variable_map, int* variable_tos, StringBuilder* code_sb, ExpressionNode* en)
{
    for (int i = 0; i < Arraylist_size(en->nodes); i++) {
        GenericNode* gn = Arraylist_get(en->nodes, i);
        int id;
        switch (gn->type)
        {
        case OperatorNode_t:
            switch(((OperatorNode*)gn->node)->op_t) {
                case Add_t:
                    StringBuilder_add_arr(code_sb, "ADD\n");
                break;
                case Sub_t:
                    StringBuilder_add_arr(code_sb, "SUB\n");
                break;
                case Mul_t:
                    StringBuilder_add_arr(code_sb, "MUL\n");
                break;
                case Div_t:
                    StringBuilder_add_arr(code_sb, "DIV\n");
                break;
                case Mod_t:
                    StringBuilder_add_arr(code_sb, "MOD\n");
                break;
                case And_t:
                    StringBuilder_add_arr(code_sb, "AND\n");
                break;
                case Or_t:
                    StringBuilder_add_arr(code_sb, "OR\n");
                break;
                case Greater_t:
                    StringBuilder_add_arr(code_sb, "CPMG\n");
                break;
                case Less_t:
                    StringBuilder_add_arr(code_sb, "CMPL\n");
                break;
                case GreaterEqual_t:
                    StringBuilder_add_arr(code_sb, "CMPG\n");
                break;
                case LessEqual_t:
                    StringBuilder_add_arr(code_sb, "CMPL\n");
                break;
                case NotEqual_t:
                    StringBuilder_add_arr(code_sb, "CMP\n");
                    StringBuilder_add_arr(code_sb, "NOT\n");
                break;
                case Not_t:
                    StringBuilder_add_arr(code_sb, "NOT\n");
                break;
                case Equal_t:
                    StringBuilder_add_arr(code_sb, "CMP\n");
                break;
            }
            break;
        case LiteralNode_t:
            id = make_literal_variable(code_sb, variable_map, variable_tos, (LiteralNode*)gn->node);
            StringBuilder_add_arr(code_sb, "PUSH ");
            StringBuilder_add_arr(code_sb, String_from_int(id));
            StringBuilder_add_arr(code_sb, "\n");
            break;
        case VariableNode_t:
            StringBuilder_add_arr(code_sb, "PUSH ");
            StringBuilder_add_arr(code_sb,String_from_int(((VariableObj*)Hashmap_get(variable_map, ((VariableNode*)gn->node)->name))->position));
            StringBuilder_add_arr(code_sb, "\n");
            break;
        case CallNode_t:
            /* code */
            break;
        default:
            break;
        }
    }
}

void parse_declaration(DeclarationNode* dn, StringBuilder* code_sb)
{
    StringBuilder_add_arr(code_sb, String_from("NEW "));
    switch(dn->type) {
    case I64_t:
    case I32_t:
    case I16_t:
    case I8_t:
    case U64_t:
    case U32_t:
    case U16_t:
    case U8_t:
    case F64_t:
    case F32_t:
        StringBuilder_add_arr(code_sb, String_from("NUM\n"));
    break;
    case Char_t:
        StringBuilder_add_arr(code_sb, String_from("STR\n"));
    break;
    }
}


void parse_assignment(AssignmentNode* an, StringBuilder* code_sb, Hashmap* variable_map, int* variable_tos)
{
    Arraylist* nodes = an->right->nodes;
    GenericNode* gn = (GenericNode*)Arraylist_get(nodes, 0);
    if (Arraylist_size(nodes) == 1) {
        LiteralNode* ln;
        switch (gn->type) {
        case LiteralNode_t:
            ln = (LiteralNode*)gn->node;
            StringBuilder_add_arr(code_sb, "SET ");
            StringBuilder_add_arr(code_sb, String_from_int(((VariableObj*)Hashmap_get(variable_map, an->left))->position));
            switch(ln->type) {
            case Char_t:
                StringBuilder_add_arr(code_sb, " STR ");
                StringBuilder_add_arr(code_sb, "\"");
                StringBuilder_add_arr(code_sb, ln->name);
                StringBuilder_add_arr(code_sb, "\"");
            break;
            default:
                StringBuilder_add_arr(code_sb, " NUM ");
                StringBuilder_add_arr(code_sb, ln->name);
            break;
            }
            
            StringBuilder_add_arr(code_sb, "\n"); 
            break;
        case CallNode_t:
            parse_call(code_sb, variable_map, variable_tos, (CallNode*)gn->node);
            StringBuilder_add_arr(code_sb, "SET ");
            StringBuilder_add_arr(code_sb, String_from_int(((VariableObj*)Hashmap_get(variable_map, an->left))->position));
            StringBuilder_add_arr(code_sb, " RET\n");
            break;
        default:
            break;
        }
    }
    else {
        parse_expression(variable_map, variable_tos, code_sb, (ExpressionNode*)an->right);
        StringBuilder_add_arr(code_sb, "POP ");
        StringBuilder_add_arr(code_sb, String_from_int(((VariableObj*)Hashmap_get(variable_map, an->left))->position));
        StringBuilder_add_arr(code_sb, "\n");
    }
}

parse_statements(Arraylist* statements, StringBuilder* code_sb, Hashmap* variable_map, int* variable_tos, int* control_id, char* fn_name)
{
    for (int i = 0; i < Arraylist_size(statements); i++) {
        GenericNode* current_statement = Arraylist_get(statements, i);
        StringBuilder* call_sb;
        StringBuilder* while_sb;
        int start;
        int loop;

        switch(current_statement->type) {
        case IfNode_t:
            parse_expression(variable_map, variable_tos, code_sb, ((IfNode*)current_statement->node)->condition);
            StringBuilder_add_arr(code_sb, "NOT\n");
            StringBuilder_add_arr(code_sb, "IFEQ CTR_L");
            StringBuilder_add_arr(code_sb, String_from_int(*control_id));
            StringBuilder_add_arr(code_sb, "\n");
            start = *variable_tos;
            parse_statements(((IfNode*)current_statement->node)->statements, code_sb, variable_map, variable_tos, control_id, fn_name);
            
            for (loop = start; loop < *variable_tos; loop++)
                StringBuilder_add_arr(code_sb, "RM\n");
            *variable_tos = start;
            
            if (i+1 < Arraylist_size(statements) && ((GenericNode*)Arraylist_get(statements, i+1))->type == ElseNode_t) {

                StringBuilder_add_arr(code_sb, "JMP CTR_L");
                StringBuilder_add_arr(code_sb, String_from_int((*control_id)+1));
                StringBuilder_add_arr(code_sb, "\n");
            }

            StringBuilder_add_arr(code_sb, "ADDR CTR_L");
            StringBuilder_add_arr(code_sb, String_from_int(*control_id));
            StringBuilder_add_arr(code_sb, "\n");
            (*control_id)++;

            break;
        case ElseNode_t:
            start = *variable_tos;
            parse_statements(((ElseNode*)current_statement->node)->statements, code_sb, variable_map, variable_tos, control_id, fn_name);
            StringBuilder_add_arr(code_sb, "ADDR CTR_L");
            StringBuilder_add_arr(code_sb, String_from_int(*control_id));
            StringBuilder_add_arr(code_sb, "\n");
            for (loop = start; loop < *variable_tos; loop++)
                StringBuilder_add_arr(code_sb, "RM\n");
            *variable_tos = start;
            (*control_id)++;
            break;
        case WhileNode_t:
            while_sb = StringBuilder_new();
            start = *variable_tos;
            parse_expression(variable_map, variable_tos, while_sb, ((WhileNode*)current_statement->node)->condition);
            StringBuilder_add_arr(while_sb, "IFEQ CTR_L");
            StringBuilder_add_arr(while_sb, String_from_int(*control_id));
            StringBuilder_add_arr(while_sb, "\n");
            
            StringBuilder_add_arr(code_sb, "JMP CTR_L");
            StringBuilder_add_arr(code_sb, String_from_int((*control_id)+1));
            StringBuilder_add_arr(code_sb, "\n");
            StringBuilder_add_arr(code_sb, "ADDR CTR_L");
            StringBuilder_add_arr(code_sb, String_from_int(*control_id));
            StringBuilder_add_arr(code_sb, "\n");
            
            parse_statements(((WhileNode*)current_statement->node)->statements, code_sb, variable_map, variable_tos, control_id, fn_name);
            for (loop = start; loop < *variable_tos; loop++)
                StringBuilder_add_arr(code_sb, "RM\n");
            *variable_tos = start;
            StringBuilder_add_arr(code_sb, "ADDR CTR_L");
            StringBuilder_add_arr(code_sb, String_from_int((*control_id)+1));
            StringBuilder_add_arr(code_sb, "\n");

            StringBuilder_add_arr(code_sb, StringBuilder_get(while_sb));
            
            
            StringBuilder_free(while_sb);

            (*control_id)+=2;

            break;
        case DeclarationNode_t:
            parse_declaration(current_statement->node, code_sb);
            Hashmap_insert(variable_map, String_from(((DeclarationNode*)current_statement->node)->name), 
                Variable_new((*variable_tos)++, ((DeclarationNode*)current_statement->node)->type));
            break;
        case AssignmentNode_t:
            parse_assignment(((AssignmentNode*)current_statement->node), code_sb, variable_map, variable_tos);
            
            break;
        case CallNode_t:
            parse_call(code_sb, variable_map, variable_tos, (CallNode*)current_statement->node);

            break;
        case ReturnNode_t:
            StringBuilder_add_arr(code_sb, String_from("COPY "));
            StringBuilder_add_arr(code_sb, String_from_int(((VariableObj*)Hashmap_get(variable_map, "RET LABEL"))->position));
            StringBuilder_add_arr(code_sb, String_from(" "));
            StringBuilder_add_arr(code_sb, String_from_int(((VariableObj*)Hashmap_get(variable_map, ((ReturnNode*)current_statement->node)->name))->position));
            StringBuilder_add_arr(code_sb, String_from("\n"));

            StringBuilder_add_arr(code_sb, String_from("JMP "));
            StringBuilder_add_arr(code_sb, String_from(fn_name));
            StringBuilder_add_arr(code_sb, String_from("_END\n"));
            break;
        default:
            panic("Invalid node: %i, report bug: https://github.com/alexburroughs/BC-2/issues", (int)current_statement->type);
            break;
        }
    }
}

void parse_imports(Arraylist* imports, StringBuilder* code_sb)
{
    for (int i = 0; i < Arraylist_size(imports); i++) {
        char* file = Arraylist_get(imports, i);
        StringBuilder* imp = StringBuilder_new();
        StringBuilder_add_arr(imp, file);
        StringBuilder_add_arr(imp, ".nnivm");
        StringBuilder_add_arr(code_sb, read_file(StringBuilder_get(imp)));
        StringBuilder_free(imp);
    }

}

void parse_function(FunctionNode* fn, StringBuilder* code_sb, int* control_id)
{

    StringBuilder_add_arr(code_sb, String_from("FS "));
    StringBuilder_add_arr(code_sb, String_from(fn->name));
    StringBuilder_add_arr(code_sb, String_from("\n"));
    Hashmap* variable_map = Hashmap_new(free);
    int variable_tos = 0;

    for (int i = 0; i < Arraylist_size(fn->args); i++) {

        Hashmap_insert(variable_map, String_from(((VariableNode*)Arraylist_get(fn->args, i))->name), 
        Variable_new(variable_tos++, ((VariableNode*)Arraylist_get(fn->args, i))->type));
    }
    if (fn->return_type != Void_t) {
       Hashmap_insert(variable_map, String_from("RET LABEL"), 
            Variable_new(variable_tos++, fn->return_type));
        parse_declaration(DeclarationNode_new(String_from("RET LABEL"), fn->return_type), code_sb);
    }

    parse_statements(fn->statements, code_sb, variable_map, &variable_tos, control_id, fn->name);
    StringBuilder_add_arr(code_sb, String_from("ADDR "));
    StringBuilder_add_arr(code_sb, String_from(fn->name));
    StringBuilder_add_arr(code_sb, String_from("_END\n"));
    StringBuilder_add_arr(code_sb, String_from("FE "));
    StringBuilder_add_arr(code_sb, String_from(fn->name));
    
    VariableObj* ret = (VariableObj*)Hashmap_get(variable_map, "RET LABEL");

    if (ret != NULL) {
        StringBuilder_add_arr(code_sb, String_from(" "));
        StringBuilder_add_arr(code_sb, String_from_int(ret->position));

    }
    StringBuilder_add_arr(code_sb, String_from("\n"));
    Hashmap_free(variable_map);
}

char* ast_to_nni(AST* ast) 
{

    Hashmap_Node* iter = Hashmap_get_iter(ast->functions);
    StringBuilder* code = StringBuilder_new();

    int control_id = 0;

    parse_imports(ast->imports, code);

    while(iter != NULL) {
        
        GenericNode* current_node = iter->val;
        if (current_node->type != FunctionNode_t) 
            panic("Invalid node: %i expected FunctionNode_t, report bug: https://github.com/alexburroughs/BC-2/issues", (int)current_node->type);
        parse_function((FunctionNode*)current_node->node, code, &control_id);
        
        iter = Hashmap_iter_next(iter);
    }

    StringBuilder_add_arr(code, "CALL main");
    char* code_str = StringBuilder_get(code);
    return code_str;
}