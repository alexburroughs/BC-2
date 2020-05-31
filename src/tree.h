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

struct expression_node
{
    Arraylist* nodes;
};

struct function_node
{
    char* name;
    Arraylist* statements;
};

struct if_node
{
    char* name;
    struct expression_node* condition;
    Arraylist* statements;
};

struct while_node
{
    char* name;
    struct expression_node* condition;
    Arraylist* statements;
};

struct declaration_node
{
    Type t;
    char* name;
};

struct assignment_node
{

};

struct call_node
{

};

typedef struct function_node FunctionNode; 
typedef struct if_node IfNode; 
typedef struct while_node WhileNode; 
typedef struct declaration_node DeclarationNode; 
typedef struct assignment_node AssignmentNode; 
typedef struct expression_node ExpressionNode;
typedef struct call_node CallNode;
typedef struct tree Tree;

Tree* Tree_new();
void Tree_free(Tree *tr);

#endif