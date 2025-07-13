#include "../headers/tree.hpp"
#include <myLib.hpp>

static void dumpListNodes   (const Node* node, FILE* file);
static void dumpConnectNodes(const Node* node, FILE* file);
static void caseOperation   (const Node* node, const char* operation, FILE* file);

Node* newNode(Type type, Value value, Node* left, Node* right)
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

    if (type == TYPE_IDENTIFIER && !value.id) 
    {
        fprintf(stderr, RED"Identifier name is null\n"RESET);
        FREE(node);
        return nullptr;
    }

    return node;
}

void dtorTree(Node* node)
{
    ASSERT(node, "node = nullptr, impossible to dtor tree with this root", stderr);

    if (node->left)  
    {
        dtorTree(node->left);
        node->left = nullptr;
    }
    if (node->right) 
    {
        dtorTree(node->right);
        node->right = nullptr;
    }    

    if (node->type == TYPE_IDENTIFIER && node->value.id) 
        FREE(node->value.id); 
    

    deleteNode(node);
}

void deleteNode(Node* node) 
{
    if (!node) return;

    node->type = (Type)0;
    node->value.num = 0;
    node->value.op = 0;

    if (node->value.id)
        node->value.id = nullptr;

    if (node->left)
        node->left = nullptr;
    
    if (node->right)
        node->right = nullptr;

    FREE(node);
}

void dumpGraph(const Node* node)
{
    ASSERT(node, "node = nullptr", stderr);

    FILE* gv = fopen(DUMP_FILE_GV, "wb");
    ASSERT(gv, "dumpGraph fopen", stderr);

    fprintf(gv, "digraph G {\n");
    fprintf(gv, "    node [style=filled, fontcolor=darkblue,"
                " fillcolor=peachpuff, color=\"#252A34\", penwidth=2.5];\n");
    fprintf(gv, "    bgcolor=\"lemonchiffon\";\n\n");

    dumpListNodes(node, gv);
    dumpConnectNodes(node, gv);
    fprintf(gv, "}\n");

    FCLOSE(gv);

    char cmd[2 * MAX_NAME_FILE_LEN + EXTRA_SPACE] = "";
    snprintf(cmd, sizeof(cmd), "dot %s -Tpng -o %s", DUMP_FILE_GV, DUMP_FILE_PNG);
    system(cmd);
}

#define DUMP_OP(op, str)  case op: caseOperation(node, str, file); break

static void dumpListNodes(const Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr", stderr);
    ASSERT(file, "dumpTreeFile = nullptr", stderr);

    if (node->type == TYPE_NUMBER)
    {
        fprintf(file,
            "    node_%p [shape=Mrecord; style = filled; fillcolor = palegreen;"
            " color = \"#000000\"; fontcolor = \"#000000\";  label=\" "
            " {NUMBER ( %lg )| addr: %llX | type: %d| value: %lg | {left: %llX | right: %llX}} \"];\n",
                        node, node->value.num, (long long unsigned int)node, (int)node->type, node->value.num,
                        (long long unsigned int)node->left, (long long unsigned int)node->right);                                                                                                   
    }
    if (node->type == TYPE_IDENTIFIER)
    {   
        fprintf(file,
            "    node_%p [shape=Mrecord; style = filled; fillcolor = cornflowerblue;"
            " color = \"#000000\"; fontcolor = \"#000000\";  label=\" "
            " {VARIABLE ( %s )| addr: %llX | type: %d| value: %s | {left: %llX | right: %llX}} \"];\n",
                        node, node->value.id, (long long unsigned int)node, (int)node->type, node->value.id, 
                        (long long unsigned int)node->left, (long long unsigned int)node->right);                                                                                                                
    }
    if (node->type == TYPE_OPERATION)
    {
        switch (node->value.op)
        {
            DUMP_OP(OPERATION_BOND, ";");
            DUMP_OP(OPERATION_FUNC_DEF, "funcDef");
            DUMP_OP(OPERATION_CALL, "call");
            DUMP_OP(OPERARION_RETURN, "return");
            DUMP_OP(OPERATION_ASSIGNMENT, "=");
            DUMP_OP(OPERATION_IF, "if");
            DUMP_OP(OPERATION_WHILE, "while");
            DUMP_OP(OPERATION_PRINT, "print");
            DUMP_OP(OPERATION_INPUT, "scan");
            DUMP_OP(OPERATION_EQUAL, "==");
            DUMP_OP(OPERATION_NOT_EQUAL, "!=");
            DUMP_OP(OPERATION_GREATER, "\\>");
            DUMP_OP(OPERATION_LESS, "\\<");
            DUMP_OP(OPERATION_GREATER_OR_EQUAL, "\\>=");
            DUMP_OP(OPERATION_LESS_OR_EQUAL, "\\<=");
            DUMP_OP(OPERATION_VAR_DEF, "varDef");
            DUMP_OP(OPERATION_SQRT, "sqrt");
            DUMP_OP(OPERATION_SIN, "sin");
            DUMP_OP(OPERATION_COS, "cos");
            DUMP_OP(OPERATION_TG, "tg");
            DUMP_OP(OPERATION_LN, "ln");
            DUMP_OP(OPERATION_ADD,   "+");
            DUMP_OP(OPERATION_SUB,   "-");
            DUMP_OP(OPERATION_MUL,   "*");
            DUMP_OP(OPERATION_DIV,   "/");
            DUMP_OP(OPERATION_POW,   "^");
            default:
            {
                fprintf(stderr, RED"ERROR IN DUMP\n"RESET);
                break;
            }
        }                                                                                      
    }
    
    if (node->left)  dumpListNodes(node->left,  file);
    if (node->right) dumpListNodes(node->right, file);
}

#undef DUMP_OP

static void caseOperation(const Node* node, const char* operation, FILE* file)
{
    ASSERT(node,      "node = nullptr",         stderr);
    ASSERT(operation, "operation = nullptr",    stderr);
    ASSERT(file,      "dumpTreeFile = nullptr", stderr);

    fprintf(file, 
        "node_%p [shape=Mrecord; style = filled; fillcolor=plum; color = \"#000000\"; fontcolor = \"#000000\";"
        "label=\" {OPERATION ( %s ) | addr: %llX | type: %d | value: %zu | {left: %llX | right: %llX}} \"];\n", 
                node, operation, (long long unsigned int)node, node->type, node->value.op, 
                (long long unsigned int)node->left, (long long unsigned int)node->right);                                                                                                          
}

static void dumpConnectNodes(const Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr", stderr);
    ASSERT(file, "file = nullptr", stderr);
    
    if (!node) return;
    
    if (node->left)  
    { 
        fprintf(file, "    node_%p -> node_%p;\n", node, node->left);  
        dumpConnectNodes(node->left, file); 
    }

    if (node->right) 
    { 
        fprintf(file, "    node_%p -> node_%p;\n", node, node->right); 
        dumpConnectNodes(node->right, file); 
    }
}

Node* copy(const Node* node)
{
    if (!node) return nullptr;

    Node* cop = (Node*)calloc(1, sizeof(Node));
    *cop = *node;                    

    if (node->type == TYPE_IDENTIFIER && node->value.id)
        cop->value.id = strdup(node->value.id);   

    cop->left  = copy(node->left);
    cop->right = copy(node->right);
    return cop;
}