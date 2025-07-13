#ifndef DSL_HPP
#define DSL_HPP

#include "../common/headers/tree.hpp"

#define OP_VALUE(oper)  Value {.op  = (oper)}
#define NUM_VALUE(n)     Value {.num = (n)   }
#define ID_VALUE(x)     Value {.id  = (x)   }

#define _OP(op_)   newNode(TYPE_OPERATION,  OP_VALUE(op_),   nullptr, nullptr)
#define _NUM(num_) newNode(TYPE_NUMBER,     NUM_VALUE(num_), nullptr, nullptr)    
#define _ID(id_)   newNode(TYPE_IDENTIFIER, ID_VALUE(id_),  nullptr, nullptr)   

#define _BOND(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_BOND), left, right)
#define _FUNC_DEF(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_FUNC_DEF), left, right)
#define _CALL(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_CALL), left, right)
#define _ASSIGNMENT(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_ASSIGNMENT),       left, right)
#define _IF(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_IF),               left, right)
#define _WHILE(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_WHILE),            left, right)

#define _EQUAL(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_EQUAL), left, right)
#define _NOT_EQUAL(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_NOT_EQUAL), left, right)
#define _GREATER(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_GREATER), left, right)
#define _LESS(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_LESS), left, right)
#define _GREATER_OR_EQUAL(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_GREATER_OR_EQUAL), left, right)
#define _LESS_OR_EQUAL(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_LESS_OR_EQUAL),    left, right)

#define _ADD(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_ADD),              left, right)
#define _SUB(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_SUB),              left, right)
#define _MUL(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_MUL),              left, right)
#define _DIV(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_DIV),              left, right)
#define _POW(left, right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_POW),              left, right)

#define _RETURN(right) newNode(TYPE_OPERATION, OP_VALUE(OPERARION_RETURN),   nullptr, right)
#define _PRINT(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_PRINT), nullptr, right)
#define _INPUT(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_INPUT),    nullptr, right)
#define _VAR_DEF(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_VAR_DEF),  nullptr, right)

#define _SQRT(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_SQRT),     nullptr, right)
#define _SIN(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_SIN),      nullptr, right)
#define _COS(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_COS),      nullptr, right)
#define _TG(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_TG),       nullptr, right)
#define _LN(right) newNode(TYPE_OPERATION, OP_VALUE(OPERATION_LN),       nullptr, right)


#endif