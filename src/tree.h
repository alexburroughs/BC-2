#ifndef TREE_H
#define TREE_H

#include "arraylist.h"
#include "hashmap.h"
#include "type.h"

enum node_type
{
    VariableNode_t,
    ExpressionNode_t,
    FunctionNode_t,
    IfNode_t,
    WhileNode_t,
    DeclarationNode_t,
    AssignmentNode_t,
    CallNode_t
};

typedef enum node_type NodeType;

struct tree 
{
    void* val;
    Hashmap* children;
};

typedef struct tree Tree;

struct generic_node
{
    void* node;
    NodeType type;
};

typedef struct generic_node GenericNode;

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
GenericNode* GenericNode_new(NodeType type, void* ptr);
void GenericNode_free(GenericNode* gn);
VariableNode* VariableNode_new(char* name, char* scope, Type type);
void VariableNode_free(VariableNode *vn);
ExpressionNode* ExpressionNode_new();
void ExpressionNode_free(ExpressionNode *en);
FunctionNode* FunctionNode_new(char* name);
void FunctionNode_free(FunctionNode *fn);
IfNode* IfNode_new(char* name, ExpressionNode* condition);
void IfNode_free(IfNode *in);
WhileNode* WhileNode_new(char* name, ExpressionNode* condition);
void WhileNode_free(WhileNode *wn);
DeclarationNode* DeclarationNode_new(char* name, Type type);
void DeclarationNode_free(DeclarationNode *dn);
AssignmentNode* AssignmentNode_new(char* left, ExpressionNode* right);
void AssignmentNode_free(AssignmentNode *an);
CallNode* CallNode_new(char* name);
void CallNode_free(CallNode *cn);

#endif