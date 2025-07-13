#include "frontend/headers/lexicalAnalysis.hpp"
#include "frontend/headers/syntaxAnalysis.hpp"
#include "middleend/headers/optimizations.hpp"
#include <myLib.hpp>

extern const char* const codeFileName;

int main()
{
    FILE* codeFile = fopen(codeFileName, "r");
    ASSERT(codeFile, "codeFile = nullptr, impossible to read", stderr);
    
    Vector tokens = lexicalAnalysis(codeFile); 
    FCLOSE(codeFile);           

    //tokenDump(tokens);

    Node* root = syntaxAnalysis(tokens);
    ASSERT(root, "root = nullptr, error in syntax analysis", stderr);

    Node* rootWithOpt = simplify(copy(root));
    ASSERT(rootWithOpt, "rootWithOpt = nullptr, error in meddle end", stderr);

    dumpGraph(rootWithOpt);

    destroyTokens(&tokens);
    dtorTree(root);
    dtorTree(rootWithOpt);

    return 0;
}