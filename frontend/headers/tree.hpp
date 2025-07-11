#ifndef TREE_HPP
#define TREE_HPP

#include <stdio.h>

enum Type
{
    TYPE_NUMBER     = 1,
    TYPE_OPERATION  = 2,
    TYPE_IDENTIFIER = 3,
    NUMBER_OF_TYPES
};

union Value
{
    double num;
    size_t op;
    char* id;
};

struct Node
{
    Type type;
    Value value;
    Node* left;
    Node* right;
};

enum Operations
{
    OPERATION_NO,               //
    OPERATION_BOND,             // ;
    OPERATION_FUNC_DEF,         // def func
    OPERATION_CALL,             // call
    OPERARION_RETURN,           // return
    OPERATION_ASSIGNMENT,       // =
    OPERATION_IF,               // if
    OPERATION_WHILE,            // while
    OPERATION_PRINT,            // print
    OPERATION_INPUT,            // scanf
    OPERATION_EQUAL,            // ==
    OPERATION_NOT_EQUAL,        // !=
    OPERATION_GREATER,          // >
    OPERATION_LESS,             // <
    OPERATION_GREATER_OR_EQUAL, // >=
    OPERATION_LESS_OR_EQUAL,    // <=
    OPERATION_VAR_DEF,          // def var
    OPERATION_SQRT,             // sqrt
    OPERATION_SIN,              // sin
    OPERATION_COS,              // cos
    OPERATION_TG,               // tg
    OPERATION_LN,               // ln
    OPERATION_ADD,              // +
    OPERATION_SUB,              // -
    OPERATION_MUL,              // *
    OPERATION_DIV,              // /
    OPERATION_POW,              // ^
    NUMBER_OF_OPERATIONS
};

Node* newNode   (Type type, Value value, Node* left, Node* right);
void  deleteNode(Node* node); 
void  dtorTree  (Node* node);



#endif