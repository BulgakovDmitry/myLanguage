#include "frontend/headers/lexicalAnalysis.hpp"
#include <myLib.hpp>

extern const char* const codeFileName;

int main()
{
    FILE* codeFile = fopen(codeFileName, "r");
    ASSERT(codeFile, "codeFile = nullptr, impossible to read", stderr);
    
    Vector tokens = splitIntoTokens(codeFile); 
    FCLOSE(codeFile);           

    tokenDump(tokens);
    destroyTokens(&tokens);

    return 0;
}