#ifndef SYNTAX_ANALYSIS_HPP
#define SYNTAX_ANALYSIS_HPP

#include <vector.hpp>
#include <myLib.hpp>
#include "tree.hpp"
#include "dsl.hpp"

Node* syntaxAnalysis(const Vector tokens);

inline void sintaxError()
{
    fprintf(stderr, RED"syntax error, please, rewrite your code\n"RESET);
    abort();
}

#endif