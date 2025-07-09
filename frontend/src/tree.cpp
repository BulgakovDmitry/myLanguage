#include "../headers/tree.hpp"
#include <myLib.hpp>

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