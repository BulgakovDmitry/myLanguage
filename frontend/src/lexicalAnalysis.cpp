#include "../headers/lexicalAnalysis.hpp"
#include "../headers/tree.hpp"
#include <myLib.hpp>
#include <ctype.h>

static bool isKeyWord(const char* word);

Vector splitIntoTokens(FILE* codeFile)
{
    ASSERT(codeFile, "codeFile = nullptr, impossible to read", stderr);

    const size_t fileSize = getFileSize(codeFile);
    if (fileSize == 0)
        return {}; 
        
    char* buffer = (char*)(calloc(fileSize + 1, sizeof(char)));
    ASSERT(buffer, "buffer = nullptr, calloc failed", stderr);

    const size_t bytesRead = fread(buffer, 1, fileSize, codeFile);
    buffer[bytesRead] = '\0'; 

    Vector tokens = {};
    vectorCtor(&tokens);

    char* curr = strtok(buffer, " \n");
    while (curr)
    {
        Node* token = (Node*)calloc(1, sizeof(Node));
        ASSERT(token, "token = nullptr, calloc error", stderr);

        token->value = curr;

        size_t currSize = tokens.size;
        if (currSize)
        {
            token->left = (Node*)vectorGet(&tokens, currSize - 1);
            ((Node*)vectorGet(&tokens, currSize - 1))->right = token; 
        }
        else
        {
            Node* serviceToken = (Node*)calloc(1, sizeof(Node));
            assert(serviceToken);

            token->left = serviceToken;

            serviceToken->value = buffer;
        }

        if (isKeyWord(curr))
            token->type = TYPE_OPERATION;
        else if (isdigit(curr[0]))
            token->type = TYPE_NUMBER;
        else
            token->type = TYPE_IDENTIFIER;

        vectorPush(&tokens, token); 
        curr = strtok(NULL, " \n");
    }

    return tokens;   
}

static bool isKeyWord(const char* word)
{
    static const char* const kKeywords[] = 
    {
        KEY_KOLI, KEY_DOKOLE, KEY_ADD,  KEY_SUB,  KEY_MUL,  KEY_DIV,
        KEY_AMIN, KEY_SIN, KEY_COS, KEY_PROPOY, KEY_ZAMYSEL, KEY_SQRT, KEY_GLAGOLI, KEY_VOZVRATISHI,
        KEY_MENE, KEY_BOLE, KEY_DON, KEY_YKO,
        KEY_NE_YKO, KEY_NE_BOLE, KEY_NE_MENE,
        KEY_LEFT_PARENTHESIS,  KEY_RIGHT_PARENTHESIS,
        KEY_LEFT_CURLY_BRACKET, KEY_RIGHT_CURLY_BRACKET, 
        KEY_POZHERTVUI, KEY_RADI
    };

    for (const char* kw : kKeywords)
    {
        if (strcmp(word, kw) == 0)
            return true;
    }

    return false;
}

void destroyTokens(Vector* tokens)
{
    if (!tokens ||tokens->size == 0) return;

    Node* firstToken    = (Node*)vectorGet(tokens, 0);
    Node* serviceToken  = firstToken ? firstToken->left : nullptr;

    if (serviceToken && serviceToken->value)
    {
        FREE(serviceToken->value);
        FREE(serviceToken);         
    }

    for (size_t i = 0; i < tokens->size; i++)
    {
        Node* tok = (Node*)vectorGet(tokens, i);
        deleteNode(tok);
    }

    vectorDtor(tokens); 
}

void tokenDump(const Vector tokens)
{    
    printf(RED"___tokenDump_____________________________________________________________\n"RESET);
    for (size_t i = 0; i < tokens.size; i++) 
    {
        const Node* tok = (Node*)vectorGet(&tokens, i);

        if (!tok) 
        {
            printf("%s[%s%zu%s] %s<null token>%s\n", GREEN, MANG, i, GREEN, RED, RESET);
            continue;
        }

        const char* val = tok->value ? tok->value : RED"<null value>"RESET;
        printf("%s[%s%zu%s] %s\"%s%s%s\"%s\n", GREEN, MANG, i, GREEN, GREEN, CEAN, val, GREEN, RESET);
    }
    printf(RED"_________________________________________________________________________\n"RESET);
}