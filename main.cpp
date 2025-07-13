#include "frontend/headers/lexicalAnalysis.hpp"
#include "frontend/headers/syntaxAnalysis.hpp"
#include <myLib.hpp>

extern const char* const codeFileName;

int main()
{
    FILE* codeFile = fopen(codeFileName, "r");
    ASSERT(codeFile, "codeFile = nullptr, impossible to read", stderr);
    
    Vector tokens = lexicalAnalysis(codeFile); 
    FCLOSE(codeFile);           

    tokenDump(tokens);

    Node* root = syntaxAnalysis(tokens);
    ASSERT(root, "root = nullptr, error in syntax analysis", stderr);

    dumpGraph(root);

    destroyTokens(&tokens);
    dtorTree(root);

    return 0;
}