#include "../headers/codeGenerator.hpp"
#include "../../common/headers/tree.hpp"

static int  getVarAddr         (const char* name);
static void genLabel           (const char* prefix, char* buf);
static void translateCondJump  (const Node* cond, const char* label, FILE* file);
static void translateNode      (const Node* node, FILE* file);
static void translateFunctions (const Node* node, FILE* file);
static void translateMain      (const Node* node, FILE* file);
static void translateInput     (const Node* node, FILE* file);
static void translatePrint     (const Node* node, FILE* file);
static void translateVarDef    (const Node* node, FILE* file);
static void translateAssignment(const Node* node, FILE* file);
static void translateBond      (const Node* node, FILE* file);
static void translateIf        (const Node* node, FILE* file);
static void translateWhile     (const Node* node, FILE* file);
static void translateCall      (const Node* node, FILE* file);
static void translateReturn    (const Node* node, FILE* file);

static void collectArgs(const Node* argNode, const char** names, size_t* pCnt);

static VarEntry gVarTable[MAX_VARS] = {};

static int gVarCount  = 0;   
static int gNextAddr  = 0;   
static int gLabelCnt  = 0;   

void translate(const Node* root, FILE* file)
{
    ASSERT(root, "root = nullptr, impossible to translate",         stderr);
    ASSERT(file, "asmfile = nullptr, impossible to write asm code", stderr);

    for (size_t i = 0; i < MAX_VARS; i++)
        ctorVarEntry(&gVarTable[i]);

    fprintf(file, "jmp main\n\n");

    translateFunctions(root, file);

    fprintf(file, "main:\n");
    translateMain(root, file);

    //fprintf(file, "dump\n");
    fprintf(file, "hlt\n");

    for (size_t i = 0; i < MAX_VARS; i++)
        dtorVarEntry(&gVarTable[i]);

    fflush(file);
    rewind(file);
}

static void translateMain(const Node *node, FILE* file)
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
    ASSERT(file, "file = nullptr, impopssible to write", stderr);

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
                        default:                                    {break;} 
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
                        case OPERATION_SQRT: {fprintf(file, "sqrt\n"); break;}
                        case OPERATION_SIN : {fprintf(file, "sin\n" ); break;}
                        case OPERATION_COS : {fprintf(file, "cos\n" ); break;}
                        case OPERATION_TG  : {fprintf(file, "tg\n"  ); break;}
                        case OPERATION_LN  : {fprintf(file, "ln\n"  ); break;}
                        default:                                      {break;}
                    }
                    break;

                case OPERATION_ASSIGNMENT: {translateAssignment(node, file); break;}
                case OPERATION_VAR_DEF:    {translateVarDef    (node, file); break;}
                case OPERATION_PRINT:      {translatePrint     (node, file); break;}
                case OPERATION_INPUT:      {translateInput     (node, file); break;}
                case OPERATION_CALL:       {translateCall      (node, file); break;}
                case OPERATION_BOND:       {translateBond      (node, file); break;}
                case OPERATION_IF:         {translateIf        (node, file); break;}
                case OPERATION_WHILE:      {translateWhile     (node, file); break;}
                case OPERARION_RETURN:     {translateReturn    (node, file); break;}
                case OPERATION_FUNC_DEF:                                    {break;}

                default:
                {
                    fprintf(stderr, RED"translate(): unknown OPERATION %d\n"RESET, (int)node->value.op);
                    break;
                }
            }
            break; 
        }
        default:
        {
            fprintf(stderr, RED"translate(): unknown node type %d\n"RESET, (int)node->type);
            break;
        }
    }
}

static int getVarAddr(const char* name)
{
    ASSERT(name, "name = nullptr, impossible to get var addr", stderr);

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
    ASSERT(prefix, "prefix = nullptr, error", stderr);
    ASSERT(buf,    "buf = nullprt, errpr",    stderr);

    snprintf(buf, LABEL_BUF_LEN, "%s%d", prefix, gLabelCnt++);
}

static void translateFunctions(const Node* node, FILE* file)
{
    if (!node) return;
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    if (node->type == TYPE_OPERATION && node->value.op == OPERATION_FUNC_DEF) 
    {
        const char* argNames[MAX_ARGS] = {0};
        size_t nArgs = 0;

        if (node->left && node->left->left)
            collectArgs(node->left->left, argNames, &nArgs);

        const char* funcName = node->left->value.id;
        fprintf(file, "%s:\n", funcName);

        int priorVarCount = gVarCount;       //Remember table size BEFORE introducing arguments
        size_t baseAddr = (size_t)gNextAddr; // Base address for current frame

        for (size_t i = 0; i < nArgs; i++) 
        {
            bool checkNumArgs = (size_t)gVarCount < MAX_VARS;
            ASSERT(checkNumArgs, "variable table overflow", stderr);
            if (!argNames[i])
            {
                fprintf(stderr, RED"translateFunctions(): NULL arg name at pos %zu in %s\n"RESET, i, funcName);
                continue;
            }

            strncpy(gVarTable[gVarCount].name, argNames[i], sizeof(gVarTable[0].name) - 1);

            gVarTable[gVarCount].name[sizeof(gVarTable[0].name) - 1] = '\0';
            gVarTable[gVarCount].addr = (int)(baseAddr + i);
            ++gVarCount;
        }

        for (size_t i = 0; i < nArgs; ++i) 
        {
            size_t addr = baseAddr + nArgs - i - 1;  
            fprintf(file, "pop [%zu]\n", addr);
        }

        gNextAddr += (int)nArgs;  // Reserve memory for args in RAM
        
        translateNode(node->right, file);

        gVarCount = priorVarCount;
        
        return;             
    }

    translateFunctions(node->left , file);
    translateFunctions(node->right, file);
}

static void translateCondJump(const Node* cond, const char* label, FILE* file)
{
    ASSERT(cond, "cond = nullptr, impossible to translate",          stderr);
    ASSERT(label, "label = nullprt, error",                          stderr);
    ASSERT(file, "file = nullptr, impopssible to write",             stderr);
    bool checkType = cond->type == TYPE_OPERATION;
    ASSERT(checkType, "node->type != oper, impossible to translate", stderr);

    translateNode(cond->left,  file);
    translateNode(cond->right, file);

    switch (cond->value.op) 
    {
        case OPERATION_EQUAL:            {fprintf(file, "jhe %s\n", label); break;}
        case OPERATION_NOT_EQUAL:        {fprintf(file, "je %s\n",  label); break;}
        case OPERATION_LESS:             {fprintf(file, "jae %s\n", label); break;}
        case OPERATION_GREATER:          {fprintf(file, "jbe %s\n", label); break;}
        case OPERATION_LESS_OR_EQUAL:    {fprintf(file, "ja %s\n",  label); break;}
        case OPERATION_GREATER_OR_EQUAL: {fprintf(file, "jb %s\n",  label); break;}
        default: 
        {
            fprintf(stderr, "translate(): unsupported comparator %d\n", (int)cond->value.op);
            break;
        }
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
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
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
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    translateNode(node->right, file);
    fprintf(file, "out\n");
}

static void translateVarDef(const Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);
    
    translateNode(node->right, file);
}

static void translateAssignment(const Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
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
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    translateNode(node->left,  file);
    translateNode(node->right, file);
}

static void translateIf(const Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
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
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
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

static void translateCall(const Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    if (node->right)
        translateNode(node->right, file);   // push arguments 

    fprintf(file, "call %s\n", node->left->value.id);
}

static void translateReturn(const Node* node, FILE* file)
{
    ASSERT(node, "node = nullptr, impossible to translate",          stderr);
    ASSERT(file, "file = nullptr, impossible to write in null file", stderr);

    if (node->right)
        translateNode(node->right, file);

    fprintf(file, "ret\n\n");
}

static void collectArgs(const Node* argNode, const char** names, size_t* pCnt)
{
    if (!argNode) return;
    ASSERT(names, "names = nullptr, impossible to get id name", stderr);
    ASSERT(pCnt,  "pCnt = nullptr, error",                      stderr);

    if (argNode->type == TYPE_OPERATION && argNode->value.op == OPERATION_BOND) 
    {
        collectArgs(argNode->left , names, pCnt);
        collectArgs(argNode->right, names, pCnt);
    } 
    else if (argNode->type == TYPE_IDENTIFIER) 
    {
        assert(*pCnt < MAX_ARGS && "too many arguments");
        names[(*pCnt)++] = argNode->value.id; // save name
    }
}