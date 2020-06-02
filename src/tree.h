#ifndef TREE_H
#define TREE_H

#include "arraylist.h"
#include "hashmap.h"
#include "type.h"

struct expression_node;

struct tree 
{
    void* val;
    Hashmap* children;
};
typedef struct tree Tree;

struct variable_node
{
    char* name;
    char* scope;
    Type type;
};
typedef struct variable_node VariableNode;

struct expression_node
{
    Arraylist* nodes;
};
typedef struct expression_node ExpressionNode;

struct function_node
{
    char* name;
    Arraylist* statements;
};
typedef struct function_node FunctionNode; 

struct if_node
{
    char* name;
    ExpressionNode* condition;
    Arraylist* statements;
};
typedef struct if_node IfNode; 

struct while_node
{
    char* name;
    ExpressionNode* condition;
    Arraylist* statements;
};
typedef struct while_node WhileNode; 

struct declaration_node
{
    Type type;
    char* name;
};
typedef struct declaration_node DeclarationNode; 

struct assignment_node
{
    char* left;
    ExpressionNode* right;
};
typedef struct assignment_node AssignmentNode; 

struct call_node
{
    char* name;
    Arraylist* args;
};
typedef struct call_node CallNode;


Tree* Tree_new();
void Tree_free(Tree *tr);

#endif