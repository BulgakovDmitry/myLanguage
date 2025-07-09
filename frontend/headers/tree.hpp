#ifndef TREE_HPP
#define TREE_HPP

enum Type
{
    TYPE_NUMBER     = 1,
    TYPE_OPERATION  = 2,
    TYPE_IDENTIFIER = 3,
};

struct Node
{
    Type type;
    char* value;
    Node* left;
    Node* right;
};

enum Operations
{
    OPERATION_NO,           //
    OPERATION_DON,          // ;
    OPERATION_ZAMYSEL,      // def
    OPERATION_PROPOY,       // call
    OPERATION_VOZVRATISHI,  // return
    OPERATION_KOLI,         // if
    OPERATION_DOKOLE,       // while
    OPERATION_DA_BUDET,     // =
    OPERATION_GLAGOLI,      // printf
    OPERATION_SQRT,         // sqrt
    OPERATION_SIN,          // sin
    OPERATION_COS,          // cos
    OPERATION_ADD,          // +
    OPERATION_SUB,          // -
    OPERATION_MUL,          // *
    OPERATION_DIV,          // /
    OPERATION_BOLE,         // >
    OPERATION_MENE,         // <
    OPERATION_NE_MENE,      // >=
    OPERATION_NE_BOLE,      // <=
    OPERATION_YKO,          // ==
    OPERATION_NE_YKO,       // !=
};

void deleteNode(Node* node); 


#endif