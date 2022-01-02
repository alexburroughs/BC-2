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
    CallNode_t,
    OperatorNode_t,
    LiteralNode_t
};

enum op_type
{
    Add_t,
    Sub_t,
    Mul_t,
    Div_t,
    And_t,
    Or_t,
    Greater_t,
    Less_t,
    GreaterEqual_t,
    LessEqual_t,
    NotEqual_t,
    Not_t,
    Equal_t,
};

typedef enum node_type NodeType;
typedef enum op_type OpType;

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
    char* type;
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
    char return_type;
    Arraylist* args;
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

struct operator_node
{
    OpType op_t;
};
typedef struct operator_node OperatorNode;

struct literal_node
{
    char* name;
};
typedef struct literal_node LiteralNode;

GenericNode* GenericNode_new(NodeType type, void* ptr);
void GenericNode_free(GenericNode* gn);
void GenericNode_add_statement(GenericNode* gn, GenericNode* statement);
VariableNode* VariableNode_new(char* name, char* scope, char* type);
void VariableNode_free(VariableNode *vn);
ExpressionNode* ExpressionNode_new();
void ExpressionNode_free(ExpressionNode *en);
FunctionNode* FunctionNode_new(char* name);
void FunctionNode_free(FunctionNode *fn);
void FunctionNode_add_arg(FunctionNode *fn, VariableNode *vn);
void FunctionNode_set_return(FunctionNode *fn, char* type);
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
OperatorNode* OperatorNode_new(OpType opt);
void OperatorNode_free(OperatorNode *op);
void ExpressionNode_add(ExpressionNode *en, GenericNode *gn);
LiteralNode* LiteralNode_new(char* name);
void LiteralNode_free(LiteralNode* ln);

#endif