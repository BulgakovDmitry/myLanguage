#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector.hpp>

const char* const codeFileName = "codeFile.txt";

const char* const KEY_DON = "don";

const char* const KEY_ZAMYSEL = "zamysel";
const char* const KEY_PROPOY = "propoy";
const char* const KEY_VOZVRATISHI = "vozvratishi";

const char* const KEY_KOLI = "koli";
const char* const KEY_DOKOLE = "dokole";

const char* const KEY_DA_BUDET = "daBudet";   
const char* const KEY_GLAGOLI = "glagoli";

const char* const KEY_SQRT = "sqrt";
const char* const KEY_SIN = "sin";
const char* const KEY_COS = "cos";
const char* const KEY_ADD = "+";
const char* const KEY_SUB = "-";
const char* const KEY_MUL = "*";
const char* const KEY_DIV = "/";

const char* const KEY_YKO = "yko";
const char* const KEY_NE_YKO = "neYko";
const char* const KEY_BOLE = "bole";
const char* const KEY_MENE = "mene";
const char* const KEY_NE_BOLE = "neBole";
const char* const KEY_NE_MENE = "neMene";

const char* const KEY_AMIN = "amin";

const char* const KEY_LEFT_PARENTHESIS = "(";
const char* const KEY_RIGHT_PARENTHESIS = ")";

const char* const KEY_LEFT_CURLY_BRACKET = "{";
const char* const KEY_RIGHT_CURLY_BRACKET = "}";

Vector splitIntoTokens(FILE* codeFile);
void   tokenDump      (const Vector tokens);
void   destroyTokens  (Vector* tokens);

#endif