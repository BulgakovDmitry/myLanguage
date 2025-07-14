#include "../headers/codeGenerator.hpp"
#include "../../common/headers/tree.hpp"

static int  getVarAddr       (const char* name);
static void genLabel         (const char* prefix, char* buf /*[LABEL_BUF_LEN]*/);
static void translateNode    (const Node* node, FILE* file);
static void translateCondJump(const Node* cond, const char* label, FILE* file);
static void emitFunctions    (const Node* node, FILE* file);
static void emitMain         (const Node* node, FILE* file);

static void translateInput     (const Node* node, FILE* file);
static void translatePrint     (const Node* node, FILE* file);
static void translateVarDef    (const Node* node, FILE* file);
static void translateAssignment(const Node* node, FILE* file);
static void translateBond      (const Node* node, FILE* file);
static void translateIf        (const Node* node, FILE* file);
static void translateWhile     (const Node* node, FILE* file);

static VarEntry gVarTable[MAX_VARS] = {};

static int      gVarCount  = 0;   /* number of used entries             */
static int      gNextAddr  = 0;   /* next free RAM cell for variables   */
static int      gLabelCnt  = 0;   /* counter for unique labels          */

void translate(const Node* root, FILE* file)
{
    ASSERT(root, "root = nullptr, impossible to translate",         stderr);
    ASSERT(file, "asmfile = nullptr, impossible to write asm code", stderr);

    for (size_t i = 0; i < MAX_VARS; i++)
        ctorVarEntry(&gVarTable[i]);

    fprintf(file, "jmp main\n\n");

    emitFunctions(root, file);

    fprintf(file, "main:\n");
    emitMain(root, file);

    fprintf(file, "dump\n");
    fprintf(file, "hlt\n");

    for (size_t i = 0; i < MAX_VARS; i++)
        dtorVarEntry(&gVarTable[i]);

    fflush(file);
    rewind(file);
}

static void emitMain(const Node *node, FILE* file)
{
    ASSERT(node, "node = nullptr, impossible to translate",         stderr);
    ASSERT(file, "asmfile = nullptr, impossible to write asm code", stderr);

    if (!node) return;

    if (node->type == TYPE_OPERATION && node->value.op == OPERATION_FUNC_DEF)
        return;                

    translateNode(node, file);
}

static void translateNode(const Node* node, FILE* file)
{
    if (!node) return;

    switch ((int)node->type) 
    {
        case TYPE_NUMBER:
        {
            fprintf(file, "push %lg\n", node->value.num);
            break;
        }
        case TYPE_IDENTIFIER: 
        {
            int addr = getVarAddr(node->value.id);
            fprintf(file, "push [%d]\n", addr);
            break;
        }
    case TYPE_OPERATION: 
    {
        switch ((int)node->value.op) 
        {
            case OPERATION_ADD:
            case OPERATION_SUB:
            case OPERATION_MUL:
            case OPERATION_DIV:
                translateNode(node->left,  file);
                translateNode(node->right, file);
                switch (node->value.op) 
                {
                    case OPERATION_ADD: {fprintf(file, "add\n"); break;}
                    case OPERATION_SUB: {fprintf(file, "sub\n"); break;}
                    case OPERATION_MUL: {fprintf(file, "mul\n"); break;}
                    case OPERATION_DIV: {fprintf(file, "div\n"); break;}
                    default: break; 
                }
                break;

            case OPERATION_SQRT:
            case OPERATION_SIN:
            case OPERATION_COS:
            case OPERATION_TG:
            case OPERATION_LN:
                translateNode(node->right, file);
                switch (node->value.op) 
                {
                    case OPERATION_SQRT: fprintf(file, "sqrt\n"); break;
                    case OPERATION_SIN : fprintf(file, "sin\n" ); break;
                    case OPERATION_COS : fprintf(file, "cos\n" ); break;
                    case OPERATION_TG  : fprintf(file, "tg\n"  ); break;
                    case OPERATION_LN  : fprintf(file, "ln\n"  ); break;
                    default: break;
                }
                break;

            case OPERATION_ASSIGNMENT: {translateAssignment(node, file); break;}
            case OPERATION_VAR_DEF:    {translateVarDef    (node, file); break;}
            case OPERATION_PRINT:      {translatePrint     (node, file); break;}
            case OPERATION_INPUT:      {translateInput     (node, file); break;}
        case OPERATION_CALL:
        {
            if (node->right)
                translateNode(node->right, file);   /* push arguments */

            fprintf(file, "call %s\n", node->left->value.id);
            break;
        }
        case OPERATION_BOND: {translateBond(node, file); break;}
        case OPERATION_IF: {translateIf(node, file); break;}
        case OPERATION_WHILE: {translateWhile(node, file); break;}
        case OPERATION_FUNC_DEF:
        {

        }
        case OPERARION_RETURN:
            if (node->right)
                translateNode(node->right, file);
            fprintf(file, "ret\n");
            break;

        default:
            fprintf(stderr, "translate(): unknown OPERATION %d\n", (int)node->value.op);
            break;
        }
        break; 
    }

    default:
        fprintf(stderr, "translate(): unknown node type %d\n", (int)node->type);
        break;
    }
}

static int getVarAddr(const char* name)
{
    for (int i = 0; i < gVarCount; i++)
        if (strcmp(gVarTable[i].name, name) == 0)
            return gVarTable[i].addr;

    bool checkCount = gVarCount < (int)MAX_VARS;
    ASSERT(checkCount, "gVarCount >= MAX_VARS, error", stderr);

    strncpy(gVarTable[gVarCount].name, name, sizeof(gVarTable[0].name) - 1);
    gVarTable[gVarCount].name[sizeof(gVarTable[0].name) - 1] = '\0';
    gVarTable[gVarCount].addr = gNextAddr;

    gVarCount++;
    return gNextAddr++;
}

static void genLabel(const char* prefix, char* buf)
{
    snprintf(buf, LABEL_BUF_LEN, "%s%d", prefix, gLabelCnt++);
}

static void emitFunctions(const Node* node, FILE* file)
{
    if (!node) return;

    if (node->type == TYPE_OPERATION && node->value.op == OPERATION_FUNC_DEF) 
    {
        const char* funcName = node->left->value.id;
        fprintf(file, "%s:\n", funcName);

        translateNode(node->right, file);
        //fprintf(file, "ret\n\n");
        return;             
    }

    emitFunctions(node->left , file);
    emitFunctions(node->right, file);
}

static void translateCondJump(const Node* cond, const char* label, FILE* file)
{
    assert(cond && cond->type == TYPE_OPERATION);

    translateNode(cond->left,  file);
    translateNode(cond->right, file);

    switch (cond->value.op) {
        case OPERATION_EQUAL:             /* a == b  ? */
            fprintf(file, "jhe %s\n", label); /* jump if not equal */
            break;
        case OPERATION_NOT_EQUAL:
            fprintf(file, "je %s\n",  label); /* jump if equal */
            break;
        case OPERATION_LESS:
            fprintf(file, "jae %s\n", label); /* jump if a >= b */
            break;
        case OPERATION_GREATER:
            fprintf(file, "jbe %s\n", label); /* jump if a <= b */
            break;
        case OPERATION_LESS_OR_EQUAL:
            fprintf(file, "ja %s\n",  label); /* jump if a > b  */
            break;
        case OPERATION_GREATER_OR_EQUAL:
            fprintf(file, "jb %s\n",  label); /* jump if a < b  */
            break;
        default:
            fprintf(stderr, "translate(): unsupported comparator %d\n", (int)cond->value.op);
            break;
    }
}

void ctorVarEntry(VarEntry* ve)
{
    ASSERT(ve, "ve = nullptr, impossible to ctor", stderr);
    memset(ve, 0, sizeof(*ve));

    ve->name = (char*)calloc(MAX_ID_NAME_LEN, sizeof(char));
    ASSERT(ve->name, "ve->name = nullptr", stderr);
}

void dtorVarEntry(VarEntry* ve)
{
    ASSERT(ve, "ve = nullptr, impossible to dtor", stderr);

    if (ve->name)
        FREE(ve->name);
        
    memset(ve, 0, sizeof(*ve));
}

static void translateInput(const Node* node, FILE* file)
{
    ASSERT(node, "ndoe = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    fprintf(file, "in\n");
    if (node->right && node->right->type == TYPE_IDENTIFIER) 
    {
        int addr = getVarAddr(node->right->value.id);
        fprintf(file, "pop [%d]\n", addr);
    }
}

static void translatePrint(const Node* node, FILE* file)
{
    ASSERT(node, "ndoe = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    translateNode(node->right, file);
    fprintf(file, "out\n");
}

static void translateVarDef(const Node* node, FILE* file)
{
    ASSERT(node, "ndoe = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);
    translateNode(node->right, file);
}

static void translateAssignment(const Node* node, FILE* file)
{
    ASSERT(node, "ndoe = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    ASSERT(node->left, "null identifier, error", stderr);
    bool checkId = node->left->type == TYPE_IDENTIFIER;
    ASSERT(checkId, "id type undefined", stderr);

    translateNode(node->right, file);
    int addr = getVarAddr(node->left->value.id);
    fprintf(file, "pop [%d]\n", addr);
}

static void translateBond(const Node* node, FILE* file)
{
    ASSERT(node, "ndoe = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    translateNode(node->left,  file);
    translateNode(node->right, file);
}

static void translateIf(const Node* node, FILE* file)
{
    ASSERT(node, "ndoe = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    char labelEnd[LABEL_BUF_LEN];
    genLabel("endIf_", labelEnd);

    translateCondJump(node->left, labelEnd, file);   // jump (if cond false) end  
    translateNode(node->right, file);                // "if" body 
    fprintf(file, "jmp %s\n", labelEnd);             // jmp end
    fprintf(file, "%s:\n", labelEnd);                // end:
}

static void translateWhile(const Node* node, FILE* file)
{
    ASSERT(node, "ndoe = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    char labelStart[LABEL_BUF_LEN];
    char labelEnd[LABEL_BUF_LEN];
    genLabel("whileStart_", labelStart);
    genLabel("whileEnd_", labelEnd);
    
    fprintf(file, "%s:\n", labelStart);            // start:
    translateCondJump(node->left, labelEnd, file); // jump (if cond false) end   
    translateNode(node->right, file);              // "while" body
    fprintf(file, "jmp %s\n", labelStart);         // jmp start
    fprintf(file, "%s:\n",  labelEnd);             // end:
}

