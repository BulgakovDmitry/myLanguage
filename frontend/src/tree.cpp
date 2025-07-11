#include "../headers/tree.hpp"
#include <myLib.hpp>

Node* newNode(Type type, char* value, Node* left, Node* right)
{
    Node* node = (Node*)calloc(1, sizeof(Node));
    ASSERT(node, "node = nullptr, calloc error", stderr);

    if (type >= NUMBER_OF_TYPES) 
    {
        fprintf(stderr, RED"ERROR: type >= NUMBER_OF_TYPES\n"RESET);
        FREE(node); 
        return nullptr;
    }

    node->type  = type;
    node->value = value;
    node->left  = left;
    node->right = right;

    return node;
}

void dtorTree(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to dtor tree with this root", stderr);

    if (node->left)  
    {
        dtorTree(node->left);
        node->left = NULL;
    }
    if (node->right) 
    {
        dtorTree(node->right);
        node->right = NULL;
    }    
    deleteNode(node);
}

void deleteNode(Node* node) 
{
    if (!node) return;

    node->type = (Type)0;

    if (node->value)
        node->value = nullptr;

    if (node->left)
        node->left = nullptr;
    
    if (node->right)
        node->right = nullptr;

    FREE(node);
}