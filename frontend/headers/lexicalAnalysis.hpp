#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector.hpp>

const char* const codeFileName = "codeFile.txt";

const char* const KEY_NO_NAME                  = "\0";            // \0
const char* const KEY_DON_NAME                 = "don";           // ;
const char* const KEY_ZAMYSEL_NAME             = "zamysel";       // func def
const char* const KEY_ZAP_NAME                 = ",";             // ,
const char* const KEY_PROPOY_NAME              = "propoy";        // func call 
const char* const KEY_VOZVRATISHI_NAME         = "vozvratishi";   // return
const char* const KEY_POZHERTVUI_NAME          = "pozhertvui";    // scan (1)
const char* const KEY_RADI_NAME                = "radi";          // scan (2)
const char* const KEY_KOLI_NAME                = "koli";          // if
const char* const KEY_DOKOLE_NAME              = "dokole";        // while
const char* const KEY_DA_NAME                  = "da";            // = (1)
const char* const KEY_PODOBNO_NAME             = "podobno";       // = (3)
const char* const KEY_BUDET_NAME               = "budet";         // = (2)
const char* const KEY_PRIBUDET_NAME            = "pribudet";      // var def
const char* const KEY_GLAGOLI_NAME             = "glagoli";       // printf (1)
const char* const KEY_YASNO_NAME               = "yasno";         // printf (2)
const char* const KEY_SQRT_NAME                = "sqrt";          // sqrt
const char* const KEY_SIN_NAME                 = "sin";           // sin
const char* const KEY_COS_NAME                 = "cos";           // cos
const char* const KEY_TG_NAME                 = "tg";             // tg
const char* const KEY_LN_NAME                  = "ln";            // ln
const char* const KEY_ADD_NAME                 = "+";             // +
const char* const KEY_SUB_NAME                 = "-";             // -
const char* const KEY_MUL_NAME                 = "*";             // *
const char* const KEY_DIV_NAME                 = "/";             // /
const char* const KEY_POW_NAME                 = "^";             // ^
const char* const KEY_EQUAL_NAME               = "==";            // ==   
const char* const KEY_NOT_EQUAL_NAME           = "!=";            // !=   
const char* const KEY_GREATER_NAME             = ">";             // >
const char* const KEY_LESS_NAME                = "<";             // <
const char* const KEY_GREATER_OR_EQUAL_NAME    = ">=";            // >=
const char* const KEY_LESS_OR_EQUAL_NAME       = "<=";            // <=
const char* const KEY_AMIN_NAME                = "amin";          // hlt
const char* const KEY_LEFT_PARENTHESIS_NAME    = "(";             // (
const char* const KEY_RIGHT_PARENTHESIS_NAME   = ")";             // )
const char* const KEY_LEFT_CURLY_BRACKET_NAME  = "{";             // {
const char* const KEY_RIGHT_CURLY_BRACKET_NAME = "}";             // }

enum TokenOperation
{
    KEY_NO_OPERATION,
    KEY_DON_OPERATION,
    KEY_ZAMYSEL_OPERATION,
    KEY_ZAP_OPERATION,
    KEY_PROPOY_OPERATION,
    KEY_VOZVRATISHI_OPERATION,
    KEY_POZHERTVUI_OPERATION,
    KEY_RADI_OPERATION,
    KEY_KOLI_OPERATION,
    KEY_DOKOLE_OPERATION,
    KEY_DA_OPERATION,
    KEY_PODOBNO_OPERATION,
    KEY_BUDET_OPERATION,
    KEY_PRIBUDET_OPERATION,
    KEY_GLAGOLI_OPERATION,
    KEY_YASNO_OPERATION,
    KEY_SQRT_OPERATION,
    KEY_SIN_OPERATION,
    KEY_COS_OPERATION,
    KEY_TG_OPERATION,
    KEY_LN_OPERATION,
    KEY_ADD_OPERATION,
    KEY_SUB_OPERATION,
    KEY_MUL_OPERATION,
    KEY_DIV_OPERATION,
    KEY_POW_OPERATION,
    KEY_EQUAL_OPERATION,
    KEY_NOT_EQUAL_OPERATION,
    KEY_GREATER_OPERATION,
    KEY_LESS_OPERATION,
    KEY_GREATER_OR_EQUAL_OPERATION,
    KEY_LESS_OR_EQUAL_OPERATION,
    KEY_AMIN_OPERATION,
    KEY_LEFT_PARENTHESIS_OPERATION,
    KEY_RIGHT_PARENTHESIS_OPERATION,
    KEY_LEFT_CURLY_BRACKET_OPERATION,
    KEY_RIGHT_CURLY_BRACKET_OPERATION,
    NUMBER_OF_KEY_OPERATION
};

struct KeyWord
{
    size_t keyWordOperation;
    const char* keyWordName;
    bool keyWordFlag;
};

Vector lexicalAnalysis(FILE* codeFile);
void   tokenDump      (const Vector tokens);
void   destroyTokens  (Vector* tokens);

#endif