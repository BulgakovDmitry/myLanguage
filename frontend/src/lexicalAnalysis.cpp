#include "../headers/lexicalAnalysis.hpp"
#include "../headers/tree.hpp"
#include <myLib.hpp>
#include <ctype.h>

static KeyWord isKeyWord(const char* word);

Vector lexicalAnalysis(FILE* codeFile)
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

        KeyWord keyWord = isKeyWord(curr);
        if (keyWord.keyWordFlag)
        {
            token->value.op = keyWord.keyWordOperation;
            token->type = TYPE_OPERATION;
        }
        else if (isdigit(curr[0]))
        {
            token->value.num = atof(curr);
            token->type = TYPE_NUMBER;
        }
        else
        {
            token->value.id = curr;
            token->type = TYPE_IDENTIFIER;
        }

        vectorPush(&tokens, token); 
        curr = strtok(NULL, " \n");
    }

    return tokens;   
}

static KeyWord isKeyWord(const char* word)
{
    static const KeyWord KEY_WORDS[NUMBER_OF_KEY_OPERATION] = 
    {
        {KEY_DON_OPERATION,                 KEY_DON_NAME,                 false}, 
        {KEY_ZAMYSEL_OPERATION,             KEY_ZAMYSEL_NAME,             false}, 
        {KEY_ZAP_OPERATION,                 KEY_ZAP_NAME,                 false}, 
        {KEY_PROPOY_OPERATION,              KEY_PROPOY_NAME,              false}, 
        {KEY_VOZVRATISHI_OPERATION,         KEY_VOZVRATISHI_NAME,         false},
        {KEY_POZHERTVUI_OPERATION,          KEY_POZHERTVUI_NAME,          false}, 
        {KEY_RADI_OPERATION,                KEY_RADI_NAME,                false}, 
        {KEY_KOLI_OPERATION,                KEY_KOLI_NAME,                false}, 
        {KEY_DOKOLE_OPERATION,              KEY_DOKOLE_NAME,              false}, 
        {KEY_DA_OPERATION,                  KEY_DA_NAME,                  false}, 
        {KEY_PODOBNO_OPERATION,             KEY_PODOBNO_NAME,             false}, 
        {KEY_BUDET_OPERATION,               KEY_BUDET_NAME,               false}, 
        {KEY_PRIBUDET_OPERATION,            KEY_PRIBUDET_NAME,            false}, 
        {KEY_GLAGOLI_OPERATION,             KEY_GLAGOLI_NAME,             false}, 
        {KEY_YASNO_OPERATION,               KEY_YASNO_NAME,               false}, 
        {KEY_SQRT_OPERATION,                KEY_SQRT_NAME,                false}, 
        {KEY_SIN_OPERATION,                 KEY_SIN_NAME,                 false}, 
        {KEY_COS_OPERATION,                 KEY_COS_NAME,                 false},
        {KEY_TG_OPERATION,                  KEY_TG_NAME,                  false}, 
        {KEY_LN_OPERATION,                  KEY_LN_NAME,                  false},
        {KEY_ADD_OPERATION,                 KEY_ADD_NAME,                 false}, 
        {KEY_SUB_OPERATION,                 KEY_SUB_NAME,                 false}, 
        {KEY_MUL_OPERATION,                 KEY_MUL_NAME,                 false}, 
        {KEY_DIV_OPERATION,                 KEY_DIV_NAME,                 false}, 
        {KEY_POW_OPERATION,                 KEY_POW_NAME,                 false},
        {KEY_EQUAL_OPERATION,               KEY_EQUAL_NAME,               false}, 
        {KEY_NOT_EQUAL_OPERATION,           KEY_NOT_EQUAL_NAME,           false}, 
        {KEY_GREATER_OPERATION,             KEY_GREATER_NAME,             false}, 
        {KEY_LESS_OPERATION,                KEY_LESS_NAME,                false}, 
        {KEY_GREATER_OR_EQUAL_OPERATION,    KEY_GREATER_OR_EQUAL_NAME,    false}, 
        {KEY_LESS_OR_EQUAL_OPERATION,       KEY_LESS_OR_EQUAL_NAME,       false}, 
        {KEY_AMIN_OPERATION,                KEY_AMIN_NAME,                false}, 
        {KEY_LEFT_PARENTHESIS_OPERATION,    KEY_LEFT_PARENTHESIS_NAME,    false}, 
        {KEY_RIGHT_PARENTHESIS_OPERATION,   KEY_RIGHT_PARENTHESIS_NAME,   false}, 
        {KEY_LEFT_CURLY_BRACKET_OPERATION,  KEY_LEFT_CURLY_BRACKET_NAME,  false}, 
        {KEY_RIGHT_CURLY_BRACKET_OPERATION, KEY_RIGHT_CURLY_BRACKET_NAME, false} 
    };

    for (int i = 0; i < NUMBER_OF_KEY_OPERATION - 1; i++)
    {
        if (strcmp(word, KEY_WORDS[i].keyWordName) == 0)
            return {KEY_WORDS[i].keyWordOperation, KEY_WORDS[i].keyWordName, true};
    }

    return {KEY_NO_OPERATION, nullptr, false};
}

void destroyTokens(Vector* tokens)
{
    if (!tokens ||tokens->size == 0) return;

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

        if (tok->type == TYPE_NUMBER)
            printf("%s[%s%zu%s] %s\"%s%lg%s\"%s\n", GREEN, MANG, i, GREEN, GREEN, CEAN, tok->value.num, GREEN, RESET);
        else if (tok->type == TYPE_OPERATION)
            printf("%s[%s%zu%s] %s\"%s%zu%s\"%s\n", GREEN, MANG, i, GREEN, GREEN, CEAN, tok->value.op, GREEN, RESET);
        else if (tok->type == TYPE_IDENTIFIER)
        {
            const char* val = tok->value.id ? tok->value.id : RED"<null value>"RESET;
            printf("%s[%s%zu%s] %s\"%s%s%s\"%s\n", GREEN, MANG, i, GREEN, GREEN, CEAN, val, GREEN, RESET);
        }
        else
        {
            fprintf(stderr, RED"undefined node type, error\n"RESET);
            return;
        }
    }
    printf(RED"_________________________________________________________________________\n"RESET);
}