#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

#include "../../common/headers/tree.hpp"
#include <myLib.hpp>

struct VarEntry
{
    char* name;  
    int   addr;      
};

const size_t MAX_ID_NAME_LEN = 32;
const size_t MAX_VARS        = 1024;
const size_t LABEL_BUF_LEN   = 32;
const size_t MAX_ARGS        = 8;

void translate(const Node *root, FILE* file);

void ctorVarEntry(VarEntry* ve);
void dtorVarEntry(VarEntry* ve);

#endif