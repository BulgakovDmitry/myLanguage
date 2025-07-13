#include "../headers/optimizations.hpp"
#include <math.h>
#include <myLib.hpp>

static bool containsVariable(Node* node);


static void replaceRightSubtree(Node** root);
static void replaceLeftSubtree (Node** root);

static bool needOptimization(Node* root);
static bool needConstFolder (Node* node);

static Node* constFolding(Node* node);

static Node* optimization   (Node* node);
static Node* optimisationAdd(Node* node);
static Node* optimisationSub(Node* node);
static Node* optimisationMul(Node* node);
static Node* optimisationDiv(Node* node);
static Node* optimisationPow(Node* node);

Node* simplify(Node* node)
{
    ASSERT(node,  "node = nullptr, impossible to simpl",           stderr);

    bool flagConstFolding  = true;
    bool flagOptimizations = true;
    
    while (flagConstFolding || flagOptimizations)
    {
        if (needConstFolder(node))
        {
            node = constFolding(node);
            flagConstFolding = true;
        }
        else 
            flagConstFolding = false;
        
        if (needOptimization(node))
        {
            node = optimization(node);
            flagOptimizations = true;
        }
        else 
            flagOptimizations = false;
    }
    
    return node;
}

static void replaceRightSubtree(Node** root)
{
    ASSERT(*root, "root = nullptr",  stderr);
    ASSERT( root, "&root = nullptr", stderr);

    Node* oldRoot = *root;
    Node* newRoot = oldRoot->right;

    oldRoot->right = nullptr;
    dtorTree(oldRoot->left);   
    oldRoot->left = nullptr;

    dtorTree(oldRoot);         
    *root = newRoot;  
}

static void replaceLeftSubtree(Node** root)
{
    ASSERT(*root, "root = nullptr", stderr);
    ASSERT(root, "&root = nullptr", stderr);
    
    Node* oldRoot = *root;
    Node* newRoot = oldRoot->left;

    oldRoot->left = nullptr;   
    dtorTree(oldRoot->right);  
    oldRoot->right = nullptr;

    dtorTree(oldRoot);
    *root = newRoot;
}

static bool needOptimization(Node* root)
{
    if (!root) return false;

    if (root->type == TYPE_OPERATION)
    {
        const bool lIsNum = root->left  && root->left ->type == TYPE_NUMBER;
        const bool rIsNum = root->right && root->right->type == TYPE_NUMBER;

        switch (root->value.op)
        {
            case OPERATION_ADD:  
            { 
                if ((lIsNum && doubleCmp(root->left ->value.num, 0)) ||
                    (rIsNum && doubleCmp(root->right->value.num, 0)))
                    return true;
                break;
            }
            case OPERATION_SUB: 
            { 
                if (rIsNum && doubleCmp(root->right->value.num, 0))
                    return true;
                break;
            }
            case OPERATION_MUL:  
            { 
                if ((lIsNum && (doubleCmp(root->left ->value.num, 0) ||
                                doubleCmp(root->left ->value.num, 1))) ||
                    (rIsNum && (doubleCmp(root->right->value.num, 0) ||
                                doubleCmp(root->right->value.num, 1))))
                    return true;
                break;
            }
            case OPERATION_DIV:
            {
                if (rIsNum && doubleCmp(root->right->value.num, 1))
                    return true;
                break;
            }
            case OPERATION_POW: 
            {
                if ((lIsNum && (doubleCmp(root->left ->value.num, 0) ||
                                doubleCmp(root->left ->value.num, 1))) ||
                    (rIsNum && (doubleCmp(root->right->value.num, 0) ||
                                doubleCmp(root->right->value.num, 1))))
                    return true;
                break;
            }
            default:
            break;          
        }
    }
    
    return needOptimization(root->left) || needOptimization(root->right);
}

static bool needConstFolder(Node* node)
{
    if (!node) return false;

    if (node->type == TYPE_OPERATION && !containsVariable(node))
        return true;                    

    return needConstFolder(node->left) || needConstFolder(node->right);
}

static Node* constFolding(Node* node)
{
    if (!node) return nullptr;

    if (node->left)  node->left  = constFolding(node->left);
    if (node->right) node->right = constFolding(node->right);

    if (node->type == TYPE_OPERATION && !containsVariable(node))
    {
        double lhs = node->left  ? node->left ->value.num : 0; 
        double rhs = node->right ? node->right->value.num : 0;
        double res = 0;
        
        switch (node->value.op)
        {
            case OPERATION_ADD:    {res = lhs + rhs;          break;}
            case OPERATION_SUB:    {res = lhs - rhs;          break;}
            case OPERATION_MUL:    {res = lhs * rhs;          break;}
            case OPERATION_DIV:    {res = lhs / rhs;          break;}
            case OPERATION_POW:    {res = pow(lhs, rhs);      break;}
            case OPERATION_SIN:    {res = sin  (rhs);         break;}
            case OPERATION_COS:    {res = cos  (rhs);         break;}
            case OPERATION_TG:     {res = tan  (rhs);         break;}
            case OPERATION_LN:     {res = log  (rhs);         break;}
            case OPERATION_SQRT:   {res = sqrt (rhs);         break;}            
            default: return node; 
        }
        
        if (node->left)  
        { 
            dtorTree(node->left);  
            node->left  = nullptr; 
        }
        if (node->right)
        { 
            dtorTree(node->right); 
            node->right = nullptr; 
        }
        
        node->type       = TYPE_NUMBER;
        node->value.num  = res;
        node->value.op   = 0;
        node->left       = nullptr;
        node->right      = nullptr;
    }
    
    return node;
}

static Node* optimization(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to optimizate", stderr);
    
    if (node->left)  node->left  = optimization(node->left);
    if (node->right) node->right = optimization(node->right);
    
    if (node->type == TYPE_OPERATION)
    {
        switch (node->value.op)
        {
            case OPERATION_ADD:
            {
                node = optimisationAdd(node);
                break;
            }    
            case OPERATION_SUB:
            {
                node = optimisationSub(node);  
                break;
            }
            case OPERATION_MUL:
            {
                node = optimisationMul(node); 
                break;
            }
            case OPERATION_DIV:
            {
                node = optimisationDiv(node);
                break;
            }
            case OPERATION_POW:
            {
                node = optimisationPow(node);
                break;
            }
            default:
                break;
        }
    }
        
    return node;
}

#define IS_NUM_ON_DIRECTION(direction, x) \
    ((node->direction) && (node->direction)->type == TYPE_NUMBER && doubleCmp((node->direction)->value.num, x))

static Node* optimisationAdd(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to do optimisationAdd", stderr);
    
    if      (IS_NUM_ON_DIRECTION(left,  0)) {replaceRightSubtree(&node);}
    else if (IS_NUM_ON_DIRECTION(right, 0)) {replaceLeftSubtree (&node);}
    
    return node;
}


static Node* optimisationSub(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to do optimisationSUB", stderr);
    
    if (IS_NUM_ON_DIRECTION(right, 0)) 
        replaceLeftSubtree(&node);
    
    return node;
}
    
static Node* optimisationMul(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to do optimisationMul", stderr);

    if      (IS_NUM_ON_DIRECTION(left,  0)) {replaceLeftSubtree (&node);}
    else if (IS_NUM_ON_DIRECTION(left,  1)) {replaceRightSubtree(&node);}
    else if (IS_NUM_ON_DIRECTION(right, 0)) {replaceRightSubtree(&node);}
    else if (IS_NUM_ON_DIRECTION(right, 1)) {replaceLeftSubtree (&node);}
    
    return node;
}

static Node* optimisationDiv(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to do optimisationDiv", stderr);

    if (IS_NUM_ON_DIRECTION(right, 1)) replaceLeftSubtree (&node);
    
    return node;
}

static Node* optimisationPow(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to do optimisationPow", stderr);
    
    if (IS_NUM_ON_DIRECTION(left, 0) || IS_NUM_ON_DIRECTION(left, 1))  
        replaceLeftSubtree(&node); 
    else if (IS_NUM_ON_DIRECTION(right, 0))
    {
        (node->right)->type = TYPE_NUMBER;
        (node->right)->value.num = 1;
        replaceRightSubtree(&node);
    }
    else if (IS_NUM_ON_DIRECTION(right, 1)) 
        replaceLeftSubtree(&node); 
    
    return node;
}

#undef IS_NUM_ON_DIRECTION

static bool containsVariable(Node* node) 
{
    if (!node) 
        return false;

    if (node->type == TYPE_IDENTIFIER) 
        return true;

    if (node->type == TYPE_OPERATION) 
    {
        if (node->left && containsVariable(node->left)) 
            return true;

        if (node->right && containsVariable(node->right)) 
            return true;
    }
    return false;
}
