#include "tree.h"
#include "panic.h"

#include <stdlib.h>

Tree* Tree_new()
{

}

void Tree_free(Tree *tr)
{

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
    default:
        panic("Invalid node, report bug: https://github.com/alexburroughs/BC-2/issues");
        break;
    }

    free(gn);
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

FunctionNode* FunctionNode_new(char* name)
{
    FunctionNode* fn = malloc(sizeof(FunctionNode));

    fn->name = name;
    fn->statements = Arraylist_new(GenericNode_free);

    return fn;
}

void FunctionNode_free(FunctionNode *fn)
{
    if (fn->name)
        free(fn->name);
    Arraylist_free(fn->statements);
    free(fn);
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
        free(an->right);

    free(an);
}

CallNode* CallNode_new(char* name)
{
    CallNode* cn = malloc(sizeof(CallNode));

    cn->name = name;
    cn->args = Arraylist_new(free);

    return cn;
}

void CallNode_free(CallNode *cn)
{
    if (cn->name)
        free(cn->name);

    Arraylist_free(cn->args);
    free(cn);
}