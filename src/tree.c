#include "tree.h"
#include "panic.h"
#include "token.h"

#include <stdlib.h>

#ifdef DEBUG

#endif

#define CORRELATE(from, to) \
case from: \
return to;

OpType from_token(TokenType tok) 
{
    switch (tok) {
        CORRELATE(Add, Add_t)
        CORRELATE(Subtract, Sub_t)
        CORRELATE(Multiply, Mul_t)
        CORRELATE(Divide, Div_t)
        CORRELATE(Modulus, Mod_t)
        CORRELATE(And, And_t)
        CORRELATE(Or, Or_t)
        CORRELATE(Greater, Greater_t)
        CORRELATE(GreaterEqual, GreaterEqual_t)
        CORRELATE(Less, Less_t)
        CORRELATE(LessEqual, LessEqual_t)
        CORRELATE(NotEqual, NotEqual_t)
        CORRELATE(Not, Not_t)
        CORRELATE(Equal, Equal_t)
        default:
            panic("Invalid operator");
    };
}

GenericNode* GenericNode_new(NodeType type, void* ptr)
{
    GenericNode* gn = malloc(sizeof(GenericNode));
    gn->node = ptr;
    gn->type = type;

    return gn;
}

void GenericNode_free(GenericNode* gn)
{
    switch (gn->type)
    {
    case VariableNode_t:
        VariableNode_free(gn->node);
        break;
    case ExpressionNode_t:
        ExpressionNode_free(gn->node);
        break;
    case FunctionNode_t:
        FunctionNode_free(gn->node);
        break;
    case IfNode_t:
        IfNode_free(gn->node);
        break;
    case ElseNode_t:
        ElseNode_free(gn->node);
        break;
    case WhileNode_t:
        WhileNode_free(gn->node);
        break;
    case DeclarationNode_t:
        DeclarationNode_free(gn->node);
        break;
    case AssignmentNode_t:
        AssignmentNode_free(gn->node);
        break;
    case CallNode_t:
        CallNode_free(gn->node);
        break;
    case OperatorNode_t:
        OperatorNode_free(gn->node);
        break;
    case LiteralNode_t:
        LiteralNode_free(gn->node);
        break;
    case ReturnNode_t:
        ReturnNode_free(gn->node);
        break;
    default:
        panic("Invalid node, report bug: https://github.com/alexburroughs/BC-2/issues");
        break;
    }

    free(gn);
}

NodeType GenericNode_get_last_node_type(GenericNode* gn)
{
    switch(gn->type) {
        case FunctionNode_t:
            return ((GenericNode*)Arraylist_get(((FunctionNode*)gn->node)->statements, Arraylist_size(((FunctionNode*)gn->node)->statements)-1))->type;
            break;
        case IfNode_t:
            return ((GenericNode*)Arraylist_get(((IfNode*)gn->node)->statements, Arraylist_size(((IfNode*)gn->node)->statements)-1))->type;
            break;
        case ElseNode_t:
            return ((GenericNode*)Arraylist_get(((ElseNode*)gn->node)->statements, Arraylist_size(((ElseNode*)gn->node)->statements)-1))->type;
            break;
        case WhileNode_t:
            return ((GenericNode*)Arraylist_get(((WhileNode*)gn->node)->statements, Arraylist_size(((WhileNode*)gn->node)->statements)-1))->type;
            break;
        default:
            panic("Invalid scope");
            break;
    }
}

void GenericNode_add_statement(GenericNode* gn, GenericNode* statement) 
{    
    switch(gn->type) {
        case FunctionNode_t:
            Arraylist_add(((FunctionNode*)gn->node)->statements, statement);
            break;
        case IfNode_t:
            Arraylist_add(((IfNode*)gn->node)->statements, statement);
            break;
        case ElseNode_t:
            Arraylist_add(((ElseNode*)gn->node)->statements, statement);
            break;
        case WhileNode_t:
            Arraylist_add(((WhileNode*)gn->node)->statements, statement);
            break;
        default:
            panic("Invalid scope");
            break;
    }
}

LiteralNode* LiteralNode_new(char* name, Type type)
{
    LiteralNode* ln = malloc(sizeof(LiteralNode));
    ln->name = name;
    ln->type = type;
    return ln;
}

void LiteralNode_free(LiteralNode* ln) 
{
    free(ln->name);
    free(ln);
}

VariableNode* VariableNode_new(char* name, char* scope, Type type)
{
    VariableNode* vn = malloc(sizeof(VariableNode));  
    vn->name = name;
    vn->scope = scope;
    vn->type = type;

    return vn;
}

void VariableNode_free(VariableNode *vn)
{
    if (vn->name)
        free(vn->name);
    if (vn->scope)
        free(vn->scope);
    free(vn);
}

ExpressionNode* ExpressionNode_new()
{
    ExpressionNode* en = malloc(sizeof(ExpressionNode));

    en->nodes = Arraylist_new(GenericNode_free);

    return en;
}

void ExpressionNode_free(ExpressionNode *en)
{
    Arraylist_free(en->nodes);
    free(en);
}

void ExpressionNode_add(ExpressionNode *en, GenericNode *gn)
{
    Arraylist_add(en->nodes, gn);
}

OperatorNode* OperatorNode_new(OpType opt)
{
    OperatorNode *op = malloc(sizeof(OperatorNode));
    op->op_t = opt;
    return op;
}

void OperatorNode_free(OperatorNode* on)
{
    free(on);
}

FunctionNode* FunctionNode_new(char* name)
{
    FunctionNode* fn = malloc(sizeof(FunctionNode));

    fn->name = name;
    fn->statements = Arraylist_new(GenericNode_free);
    fn->args = Arraylist_new(VariableNode_free);
    fn->return_type = 0;

    return fn;
}

void FunctionNode_free(FunctionNode *fn)
{
    if (fn->name)
        free(fn->name);
    Arraylist_free(fn->statements);
    free(fn);
}

void FunctionNode_add_arg(FunctionNode *fn, VariableNode *vn)
{
    Arraylist_add(fn->args, vn);
}

void FunctionNode_set_return(FunctionNode *fn, Type type)
{
    fn->return_type = type;
}

IfNode* IfNode_new(char* name, ExpressionNode* en)
{
    IfNode* in = malloc(sizeof(IfNode));

    in->name = name;
    in->statements = Arraylist_new(GenericNode_free);
    in->condition = en;
    
    return in;
}

void IfNode_free(IfNode *in)
{
    if (in->name)
        free(in->name);

    Arraylist_free(in->statements);
    ExpressionNode_free(in->condition);
    free(in);
}

ElseNode* ElseNode_new(char* name)
{
    ElseNode* en = malloc(sizeof(ElseNode));

    en->name = name;
    en->statements = Arraylist_new(GenericNode_free);
    
    return en;
}

void ElseNode_free(ElseNode *en)
{
    if (en->name)
        free(en->name);

    Arraylist_free(en->statements);
    free(en);
}

WhileNode* WhileNode_new(char* name, ExpressionNode* condition)
{
    WhileNode* wn = malloc(sizeof(WhileNode));

    wn->name = name;
    wn->condition = condition;
    wn->statements = Arraylist_new(GenericNode_free);

    return wn;
}

void WhileNode_free(WhileNode *wn)
{
    if (wn->name)
        free(wn->name);

    Arraylist_free(wn->statements);
    ExpressionNode_free(wn->condition);
    free(wn);
}

DeclarationNode* DeclarationNode_new(char* name, Type type)
{
    DeclarationNode* dn = malloc(sizeof(DeclarationNode));

    dn->name = name;
    dn->type = type;

    return dn;
}

void DeclarationNode_free(DeclarationNode *dn)
{
    if (dn->name)
        free(dn->name);

    free(dn);
}

AssignmentNode* AssignmentNode_new(char* left, ExpressionNode* right)
{
    AssignmentNode* an = malloc(sizeof(AssignmentNode));

    an->left = left;
    an->right = right;

    return an;
}

void AssignmentNode_free(AssignmentNode *an)
{
    if (an->left)
        free(an->left);
    if (an->right)
        ExpressionNode_free(an->right);

    free(an);
}

CallNode* CallNode_new(char* name)
{
    CallNode* cn = malloc(sizeof(CallNode));

    cn->name = name;
    cn->args = Arraylist_new(free);

    return cn;
}

void CallNode_add_arg(CallNode *cn, GenericNode *gn)
{
    Arraylist_add(cn->args, gn);
}

void CallNode_free(CallNode *cn)
{
    if (cn->name)
        free(cn->name);

    Arraylist_free(cn->args);
    free(cn);
}

ReturnNode* ReturnNode_new(char* name)
{
    ReturnNode* rn = malloc(sizeof(ReturnNode));
    rn->name = name;

    return rn;
}

void ReturnNode_free(ReturnNode* rn) 
{
    free(rn->name);
    free(rn);
}
