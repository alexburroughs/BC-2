#ifndef TREE_H
#define TREE_H

#include "arraylist.h"
#include "hashmap.h"
#include "type.h"
#include "token.h"

enum node_type
{
    VariableNode_t, //0
    ExpressionNode_t, //1
    FunctionNode_t, //2
    IfNode_t, //3
    WhileNode_t, //4
    DeclarationNode_t, //5
    AssignmentNode_t, //6
    CallNode_t, //7
    OperatorNode_t, //8
    LiteralNode_t, //9
    ElseNode_t,
    ReturnNode_t
};

enum op_type
{
    Add_t, //0
    Sub_t, //1
    Mul_t, //2
    Div_t, //3
    Mod_t, //4
    And_t, //5
    Or_t, //6
    Greater_t, //7
    Less_t, //8
    GreaterEqual_t, //9
    LessEqual_t, //10
    NotEqual_t, //11
    Not_t, //12
    Equal_t, //13
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
    Type return_type;
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

struct else_node
{
    char* name;
    Arraylist* statements;
};
typedef struct else_node ElseNode; 

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
    Type type;
};
typedef struct literal_node LiteralNode;

struct return_node
{
    char* name;
};
typedef struct return_node ReturnNode;

OpType from_token(TokenType tok);
GenericNode* GenericNode_new(NodeType type, void* ptr);
void GenericNode_free(GenericNode* gn);
void GenericNode_add_statement(GenericNode* gn, GenericNode* statement);
NodeType GenericNode_get_last_node_type(GenericNode* gn);
VariableNode* VariableNode_new(char* name, char* scope, Type type);
void VariableNode_free(VariableNode *vn);
ExpressionNode* ExpressionNode_new();
void ExpressionNode_free(ExpressionNode *en);
FunctionNode* FunctionNode_new(char* name);
void FunctionNode_free(FunctionNode *fn);
void FunctionNode_add_arg(FunctionNode *fn, VariableNode *vn);
void FunctionNode_set_return(FunctionNode *fn, Type type);
IfNode* IfNode_new(char* name, ExpressionNode* condition);
void IfNode_free(IfNode *in);
ElseNode* ElseNode_new(char* name);
void ElseNode_free(ElseNode *en);
WhileNode* WhileNode_new(char* name, ExpressionNode* condition);
void WhileNode_free(WhileNode *wn);
DeclarationNode* DeclarationNode_new(char* name, Type type);
void DeclarationNode_free(DeclarationNode *dn);
AssignmentNode* AssignmentNode_new(char* left, ExpressionNode* right);
void AssignmentNode_free(AssignmentNode *an);
CallNode* CallNode_new(char* name);
void CallNode_free(CallNode *cn);
void CallNode_add_arg(CallNode *cn, GenericNode *gn);
OperatorNode* OperatorNode_new(OpType opt);
void OperatorNode_free(OperatorNode *op);
void ExpressionNode_add(ExpressionNode *en, GenericNode *gn);
LiteralNode* LiteralNode_new(char* name, Type type);
void LiteralNode_free(LiteralNode* ln);
ReturnNode* ReturnNode_new(char* name);
void ReturnNode_free(ReturnNode* rn) ;
#endif