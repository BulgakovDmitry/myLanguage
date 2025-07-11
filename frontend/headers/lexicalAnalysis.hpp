#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector.hpp>

const char* const codeFileName = "codeFile.txt";

const char* const KEY_DON                 = "don";           // ;
const char* const KEY_ZAMYSEL             = "zamysel";       // func def
const char* const KEY_ZAP                 = ",";             // ,
const char* const KEY_PROPOY              = "propoy";        // func call 
const char* const KEY_VOZVRATISHI         = "vozvratishi";   // return
const char* const KEY_POZHERTVUI          = "pozhertvui";    // scan (1)
const char* const KEY_RADI                = "radi";          // scan (2)
const char* const KEY_KOLI                = "koli";          // if
const char* const KEY_DOKOLE              = "dokole";        // while
const char* const KEY_DA                  = "da";            // = (1)
const char* const KEY_PODOBNO             = "podobno";       // = (3)
const char* const KEY_BUDET               = "budet";         // = (2)
const char* const KEY_PRIBUDET            = "pribudet";      // var def
const char* const KEY_GLAGOLI             = "glagoli";       // printf (1)
const char* const KEY_YASNO               = "yasno";         // printf (2)
const char* const KEY_SQRT                = "sqrt";          // sqrt
const char* const KEY_SIN                 = "sin";           // sin
const char* const KEY_COS                 = "cos";           // cos
const char* const KEY_TG                  = "tg";            // tg
const char* const KEY_LN                  = "ln";            // ln
const char* const KEY_ADD                 = "+";             // +
const char* const KEY_SUB                 = "-";             // -
const char* const KEY_MUL                 = "*";             // *
const char* const KEY_DIV                 = "/";             // /
const char* const KEY_EQUAL               = "==";            // ==   
const char* const KEY_NOT_EQUAL           = "!=";            // !=   
const char* const KEY_GREATER             = ">";             // >
const char* const KEY_LESS                = "<";             // <
const char* const KEY_GREATER_OR_EQUAL    = ">=";            // >=
const char* const KEY_LESS_OR_EQUAL       = "<=";            // <=
const char* const KEY_AMIN                = "amin";          // hlt
const char* const KEY_LEFT_PARENTHESIS    = "(";             // (
const char* const KEY_RIGHT_PARENTHESIS   = ")";             // )
const char* const KEY_LEFT_CURLY_BRACKET  = "{";             // {
const char* const KEY_RIGHT_CURLY_BRACKET = "}";             // }

Vector splitIntoTokens(FILE* codeFile);
void   tokenDump      (const Vector tokens);
void   destroyTokens  (Vector* tokens);

#endif