#include "../headers/tree.hpp"
#include <myLib.hpp>

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

/*void dumpGraph(Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr", stderr);
    ASSERT(file, "file = nullptr", stderr);


    //snprintf(gvPath,  sizeof(gvPath),  "dumpGraph/%s.gv",  filePrefix);
    //snprintf(pngPath, sizeof(pngPath), "dumpGraph/%s.png", filePrefix);

    FILE* gv = fopen("dumpGraph.gv", "wb");
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
    snprintf(cmd, sizeof(cmd), "dot %s -Tpng -o %s", gvPath, pngPath);
    system(cmd);

    fprintf(html,
        "<div class=\"pair\">"
        "<img src=\"%s.png\" alt=\"%s\">"
        "<span>it's %s</span>"
        "</div>\n",
        filePrefix, filePrefix, filePrefix);
}*/