#include "../headers/syntaxAnalysis.hpp"
#include "../headers/lexicalAnalysis.hpp"
#include "../headers/dsl.hpp"
#include <ctype.h>

/*------------------------------___GRAMMAR___------------------------------*/
// Program     ::= StmtList 'amin'
// StmtList    ::= { Statement 'don' }
//
// Statement   ::= Assignment | IfStmt  | WhileStmt | 
//                 VarDef     | FuncDef | FuncCall  | 
//                 Return     | Input   | Print
//
// IfStmt     ::= 'koli'   Expression '{' StmtList '}'
// WhileStmt  ::= 'dokole' Expression '{' StmtList '}'
//
// Assignment ::= 'da' 'budet'    Var 'podobno' Expression
// VarDef     ::= 'da' 'pribudet' Var 'podobno' Expression 
// FuncDef    ::= 'zamysel' ID '(' [ ParamList ] ')' '{' StmtList '}'
// ParamList  ::= Var { ',' Var }
//
// Return     ::= 'vozvratishi' Expression
// Input      ::= 'pozhertvui' 'radi' Var
// Print      ::= 'glagoli' 'yasno' Expression
// FuncCall   ::= ID '(' [ ArgList ] ')'
// ArgList    ::= Expression { ',' Expression }
//
// Expression ::= Equality
// Equality   ::= Rel ( ( '==' | '!=' ) Rel )*
// Rel        ::= AddSub ( ( '<' | '>' | '<=' | '>=' ) AddSub )*
// AddSub     ::= MulDiv ( ( '+' | '-' ) MulDiv )*
// MulDiv     ::= Pow ( ( '*' | '/' ) Pow )*
// Pow        ::= Unary ('^' Unary )*
// Unary      ::= ( '+' | '-' | FuncOper )? Primary
// FuncOper   ::= 'sin' | 'cos' | 'tg' | 'ln' | 'sqrt'
// Primary    ::= '(' Expression ')'| Var | Number
//
// Var        ::= ID
// ID         ::= ([a-z] | [A-Z])+
// Num        ::= ['0'-'9']+ 
/*-------------------------------------------------------------------------*/

static Node* getProgram   (size_t* pos, const Vector tokens);
static Node* getStmtList  (size_t* pos, const Vector tokens);
static Node* getStatement (size_t* pos, const Vector tokens);
static Node* getIfStmt    (size_t* pos, const Vector tokens);
static Node* getWhileStmt (size_t* pos, const Vector tokens);
static Node* getAssignment(size_t* pos, const Vector tokens);
static Node* getVarDef    (size_t* pos, const Vector tokens);
static Node* getFuncDef   (size_t* pos, const Vector tokens);
static Node* getParamList (size_t* pos, const Vector tokens);
static Node* getReturn    (size_t* pos, const Vector tokens);
static Node* getInput     (size_t* pos, const Vector tokens);
static Node* getPrint     (size_t* pos, const Vector tokens);
static Node* getFuncCall  (size_t* pos, const Vector tokens);
static Node* getArgList   (size_t* pos, const Vector tokens);
static Node* getExpression(size_t* pos, const Vector tokens);
static Node* getEquality  (size_t* pos, const Vector tokens);
static Node* getRel       (size_t* pos, const Vector tokens);
static Node* getAddSub    (size_t* pos, const Vector tokens);
static Node* getMulDiv    (size_t* pos, const Vector tokens);
static Node* getPow       (size_t* pos, const Vector tokens);
static Node* getUnary     (size_t* pos, const Vector tokens);
static Node* getFuncOper  (size_t* pos, const Vector tokens);
static Node* getPrimary   (size_t* pos, const Vector tokens);

#define CUR()           ( ((Node*)vectorGet(&tokens, *pos)) )      
#define NEXT()          ( ((Node*)vectorGet(&tokens, *pos + 1)) )    
#define IS(type)        ( CUR()->value.op == (type) )

#define REQUIRE(type)      \
    do                     \
    {                      \
        if (!IS(type))     \
        {                  \
            sintaxError(); \
            return nullptr;\
        }                  \
        (*pos)++;          \
    } while(0)          

static inline bool isNumber(const Node* node) 
{
    if (!node) return false; 
    return node->type == TYPE_NUMBER; 
}

static inline bool isIdentifier(const Node* node) 
{
    if (!node) return false;
    return node->type == TYPE_IDENTIFIER && node->value.id != nullptr;
}

Node* syntaxAnalysis(const Vector tokens)
{    
    size_t pos = 0;

    if (tokens.size == 0)
        return nullptr;

    
    Node* root = getProgram(&pos, tokens);
    
    if (pos != tokens.size)
    {
        fprintf(stderr, RED"not all tokens haм been read\n"RESET);
        return nullptr;
    }

    return root;
}

//------------------------------------------------------------------------------
//  Program ::= StmtList 'amin'
//------------------------------------------------------------------------------
static Node* getProgram(size_t* pos, const Vector tokens)
{
    Node* node = getStmtList(pos, tokens);
    REQUIRE(KEY_AMIN_OPERATION);
    return node;
}

//------------------------------------------------------------------------------
//  StmtList ::= { Statement 'don' }
//------------------------------------------------------------------------------
static Node* getStmtList(size_t* pos, const Vector tokens)
{
    Node* list = nullptr;

    while (*pos < tokens.size && !IS(KEY_AMIN_OPERATION) && !IS(KEY_RIGHT_CURLY_BRACKET_OPERATION))
    {
        Node* stmt = getStatement(pos, tokens);
        if (*pos >= tokens.size) break;
        REQUIRE(KEY_DON_OPERATION);

        if (!list) list = stmt;
        else       list = _BOND(list, stmt);
    }
    return list;
}

//------------------------------------------------------------------------------
// Statement ::= Assignment | IfStmt  | WhileStmt | 
//               VarDef     | FuncDef | FuncCall  | 
//               Return     | Input   | Print
//------------------------------------------------------------------------------
static Node* getStatement(size_t* pos, const Vector tokens)
{
    if      (IS(KEY_KOLI_OPERATION))   return getIfStmt   (pos, tokens);
    else if (IS(KEY_DOKOLE_OPERATION)) return getWhileStmt(pos, tokens);
    else if (IS(KEY_DA_OPERATION))
    {
        if (*pos + 1 >= tokens.size)                          return nullptr;
        if      (NEXT()->value.op == KEY_BUDET_OPERATION)     return getAssignment(pos, tokens);
        else if (NEXT()->value.op == KEY_PRIBUDET_OPERATION)  return getVarDef    (pos, tokens);
        else    {sintaxError(); return nullptr;}
    }
    else if (IS(KEY_ZAMYSEL_OPERATION))     return getFuncDef  (pos, tokens);
    else if (IS(KEY_VOZVRATISHI_OPERATION)) return getReturn   (pos, tokens);
    else if (IS(KEY_POZHERTVUI_OPERATION))  return getInput    (pos, tokens);
    else if (IS(KEY_GLAGOLI_OPERATION))     return getPrint    (pos, tokens);
    else if (isIdentifier(CUR()) && NEXT()->value.op == KEY_LEFT_PARENTHESIS_OPERATION)
                                            return getFuncCall (pos, tokens);
    else {sintaxError(); return nullptr;}

    return nullptr; 
}

//------------------------------------------------------------------------------
//  IfStmt ::= 'koli' Expression '{' StmtList '}'
//------------------------------------------------------------------------------
static Node* getIfStmt(size_t* pos, const Vector tokens)
{
    REQUIRE(KEY_KOLI_OPERATION);
    Node* cond = getExpression(pos, tokens);
    REQUIRE(KEY_LEFT_CURLY_BRACKET_OPERATION);
    Node* body = getStmtList(pos, tokens);
    REQUIRE(KEY_RIGHT_CURLY_BRACKET_OPERATION);

    return _IF(cond, body);
}

//------------------------------------------------------------------------------
//  WhileStmt ::= 'dokole' Expression '{' StmtList '}'
//------------------------------------------------------------------------------
static Node* getWhileStmt(size_t* pos, const Vector tokens)
{
    REQUIRE(KEY_DOKOLE_OPERATION);
    Node* cond = getExpression(pos, tokens);
    REQUIRE(KEY_LEFT_CURLY_BRACKET_OPERATION);
    Node* body = getStmtList(pos, tokens);
    REQUIRE(KEY_RIGHT_CURLY_BRACKET_OPERATION);

    return _WHILE(cond, body);
}

//------------------------------------------------------------------------------
//  Assignment ::= 'da' 'budet' Var 'podobno' Expression
//------------------------------------------------------------------------------
static Node* getAssignment(size_t* pos, const Vector tokens)
{
    PRINT(4);
    REQUIRE(KEY_DA_OPERATION);
    REQUIRE(KEY_BUDET_OPERATION);

    Node* idTok = CUR();
    Node* var   = _ID(strdup(idTok->value.id));
    (*pos)++;

    REQUIRE(KEY_PODOBNO_OPERATION);
    Node* expr = getExpression(pos, tokens);

    return _ASSIGNMENT(var, expr);
}

//------------------------------------------------------------------------------
//  VarDef ::= 'da' 'pribudet' Var 'podobno' Expression 
//------------------------------------------------------------------------------
static Node* getVarDef(size_t* pos, const Vector tokens)
{
    REQUIRE(KEY_DA_OPERATION);
    REQUIRE(KEY_PRIBUDET_OPERATION);

    Node* idTok = CUR();
    Node* var   = _ID(strdup(idTok->value.id));
    (*pos)++;
    if (*pos >= tokens.size || !IS(KEY_PODOBNO_OPERATION)) 
    {
        sintaxError();
        return nullptr;
    }
    REQUIRE(KEY_PODOBNO_OPERATION);
    Node* expr = getExpression(pos, tokens);

    return _VAR_DEF(_ASSIGNMENT(var, expr));
}

//------------------------------------------------------------------------------
//  FuncDef ::= 'zamysel' ID '(' [ ParamList ] ')' '{' StmtList '}'
//------------------------------------------------------------------------------
static Node* getFuncDef(size_t* pos, const Vector tokens)
{
    REQUIRE(KEY_ZAMYSEL_OPERATION);

    Node* idTok  = CUR();

    char* fName = strdup(idTok->value.id);
    if (!fName) 
    {
        fprintf(stderr, RED"Memory allocation failed\n"RED);
        sintaxError();
        return nullptr;
    }

    Node*  idNode = _ID(fName);
    (*pos)++;              

    REQUIRE(KEY_LEFT_PARENTHESIS_OPERATION);
    Node* params = nullptr;
    if (!IS(KEY_RIGHT_PARENTHESIS_OPERATION))
        params = getParamList(pos, tokens);
    REQUIRE(KEY_RIGHT_PARENTHESIS_OPERATION);

    REQUIRE(KEY_LEFT_CURLY_BRACKET_OPERATION);
    Node* body = getStmtList(pos, tokens);
    REQUIRE(KEY_RIGHT_CURLY_BRACKET_OPERATION);

    Node* def = _FUNC_DEF(params, body);
    def->left = idNode;                      
    return def;
}

//------------------------------------------------------------------------------
//  ParamList ::= Var { ',' Var }
//------------------------------------------------------------------------------
static Node* getParamList(size_t* pos, const Vector tokens)
{
    Node* first = _ID(strdup(CUR()->value.id));
    (*pos)++;

    Node* list = first;
    while (IS(KEY_ZAP_OPERATION))
    {
        (*pos)++;;
        Node* nxt = _ID(strdup(CUR()->value.id));
        (*pos)++;
        list = _BOND(list, nxt);
    }
    return list;
}

//------------------------------------------------------------------------------
//  Return ::= 'vozvratishi' Expression
//------------------------------------------------------------------------------
static Node* getReturn(size_t* pos, const Vector tokens)
{
    REQUIRE(KEY_VOZVRATISHI_OPERATION);
    Node* expr = getExpression(pos, tokens);
    return _RETURN(expr);
}

//------------------------------------------------------------------------------
//  Input ::= 'pozhertvui' 'radi' Var 
//------------------------------------------------------------------------------
static Node* getInput(size_t* pos, const Vector tokens)
{
    REQUIRE(KEY_POZHERTVUI_OPERATION);
    REQUIRE(KEY_RADI_OPERATION);
    Node* idTok = CUR();
    Node* var    = _ID(strdup(idTok->value.id));
    (*pos)++;
    return _INPUT(var);
}

//------------------------------------------------------------------------------
// Print ::= 'glagoli' 'yasno' Expression 
//------------------------------------------------------------------------------
static Node* getPrint(size_t* pos, const Vector tokens)
{
    REQUIRE(KEY_GLAGOLI_OPERATION);
    REQUIRE(KEY_YASNO_OPERATION);
    Node* expr = getExpression(pos, tokens);
    return _PRINT(expr);
}

//------------------------------------------------------------------------------
//  FuncCall ::= ID '(' [ ArgList ] ')'
//------------------------------------------------------------------------------
static Node* getFuncCall(size_t* pos, const Vector tokens)
{
    Node* idTok = CUR();
    Node* callee = _ID(strdup(idTok->value.id));
    (*pos)++;

    REQUIRE(KEY_LEFT_PARENTHESIS_OPERATION);
    Node* args = nullptr;
    if (!IS(KEY_RIGHT_PARENTHESIS_OPERATION))
        args = getArgList(pos, tokens);
    REQUIRE(KEY_RIGHT_PARENTHESIS_OPERATION);

    return _CALL(callee, args);
}

//------------------------------------------------------------------------------
//  ArgList ::= Expression { ',' Expression }
//------------------------------------------------------------------------------
static Node* getArgList(size_t* pos, const Vector tokens)
{
    Node* first = getExpression(pos, tokens);
    Node* list  = first;

    while (IS(KEY_ZAP_OPERATION))
    {
        (*pos)++;;
        Node* nxt = getExpression(pos, tokens);
        list = _BOND(list, nxt);
    }
    return list;
}

//------------------------------------------------------------------------------
//  Expression ::= Equality
//------------------------------------------------------------------------------
static Node* getExpression(size_t* pos, const Vector tokens) { return getEquality(pos, tokens); }

//------------------------------------------------------------------------------
//  Equality ::= Rel ( ( '==' | '!=' ) Rel )*
//------------------------------------------------------------------------------
static Node* getEquality(size_t* pos, const Vector tokens)
{
    Node* node = getRel(pos, tokens);
    while (IS(KEY_EQUAL_OPERATION) || IS(KEY_NOT_EQUAL_OPERATION))
    {
        size_t op = CUR()->value.op; (*pos)++;;
        Node* rhs = getRel(pos, tokens);
        node = (op == KEY_EQUAL_OPERATION) ? _EQUAL(node, rhs) : _NOT_EQUAL(node, rhs);
    }
    return node;
}

//------------------------------------------------------------------------------
//  Rel ::= AddSub ( ( '<' | '>' | '<=' | '>=' ) AddSub )*
//------------------------------------------------------------------------------
static Node* getRel(size_t* pos, const Vector tokens)
{
    Node* node = getAddSub(pos, tokens);
    while (IS(KEY_LESS_OPERATION) || IS(KEY_GREATER_OPERATION) ||
           IS(KEY_LESS_OR_EQUAL_OPERATION) || IS(KEY_GREATER_OR_EQUAL_OPERATION))
    {
        size_t op = CUR()->value.op; (*pos)++;;
        Node* rhs = getAddSub(pos, tokens);
        switch (op)
        {
            case KEY_LESS_OPERATION:              node = _LESS(node, rhs);              break;
            case KEY_GREATER_OPERATION:           node = _GREATER(node, rhs);           break;
            case KEY_LESS_OR_EQUAL_OPERATION:     node = _LESS_OR_EQUAL(node, rhs);     break;
            case KEY_GREATER_OR_EQUAL_OPERATION:  node = _GREATER_OR_EQUAL(node, rhs);  break;
            default: {sintaxError(); return nullptr;}
        }
    }
    return node;
}

//------------------------------------------------------------------------------
//  AddSub ::= MulDiv ( ( '+' | '-' ) MulDiv )*
//------------------------------------------------------------------------------
static Node* getAddSub(size_t* pos, const Vector tokens)
{
    Node* node = getMulDiv(pos, tokens);
    while (IS(KEY_ADD_OPERATION) || IS(KEY_SUB_OPERATION))
    {
        size_t op = CUR()->value.op; (*pos)++;;
        Node* rhs = getMulDiv(pos, tokens);
        node = (op == KEY_ADD_OPERATION) ? _ADD(node, rhs) : _SUB(node, rhs);
    }
    return node;
}

//------------------------------------------------------------------------------
//  MulDiv ::= Pow ( ( '*' | '/' ) Pow )*
//------------------------------------------------------------------------------
static Node* getMulDiv(size_t* pos, const Vector tokens)
{
    Node* node = getPow(pos, tokens);
    while (IS(KEY_MUL_OPERATION) || IS(KEY_DIV_OPERATION))
    {
        size_t op = CUR()->value.op; (*pos)++;;
        Node* rhs = getPow(pos, tokens);
        node = (op == KEY_MUL_OPERATION) ? _MUL(node, rhs) : _DIV(node, rhs);
    }
    return node;
}

//------------------------------------------------------------------------------
//  Pow ::= Unary ('^' Unary )*
//------------------------------------------------------------------------------
static Node* getPow(size_t* pos, const Vector tokens)
{
    Node* node = getUnary(pos, tokens);
    while (IS(KEY_POW_OPERATION))
    {
        (*pos)++;;
        Node* rhs = getUnary(pos, tokens);
        node = _POW(node, rhs);
    }
    return node;
}

//------------------------------------------------------------------------------
//  Unary ::= ( '+' | '-' | FuncOper )? Primary 
//------------------------------------------------------------------------------
static Node* getUnary(size_t* pos, const Vector tokens)
{
    if (IS(KEY_ADD_OPERATION))            { (*pos)++;return getUnary(pos, tokens); }
    else if (IS(KEY_SUB_OPERATION))       { (*pos)++;return _SUB(_NUM(0), getUnary(pos, tokens)); }
    else if (IS(KEY_SIN_OPERATION)  || IS(KEY_COS_OPERATION) || IS(KEY_TG_OPERATION) ||
             IS(KEY_LN_OPERATION)   || IS(KEY_SQRT_OPERATION))
        return getFuncOper(pos, tokens);
    else
        return getPrimary(pos, tokens);
}

//------------------------------------------------------------------------------
//  FuncOper ::= 'sin' | 'cos' | 'tg' | 'ln' | 'sqrt' 
//------------------------------------------------------------------------------
static Node* getFuncOper(size_t* pos, const Vector tokens)
{
    size_t op = CUR()->value.op; (*pos)++;;
    Node* arg = getPrimary(pos, tokens);
    switch (op)
    {
        case KEY_SIN_OPERATION:  return _SIN (arg);
        case KEY_COS_OPERATION:  return _COS (arg);
        case KEY_TG_OPERATION:   return _TG  (arg);
        case KEY_LN_OPERATION:   return _LN  (arg);
        case KEY_SQRT_OPERATION: return _SQRT(arg);
        default: {sintaxError(); return nullptr;}
    }
}

//------------------------------------------------------------------------------
//  Primary ::= '(' Expression ')'| Var | Number 
//------------------------------------------------------------------------------
static Node* getPrimary(size_t* pos, const Vector tokens)
{
    if (*pos >= tokens.size || !CUR()) 
    {
        sintaxError();
        return nullptr;
    }

    if (IS(KEY_LEFT_PARENTHESIS_OPERATION))
    {
        (*pos)++;;
        Node* expr = getExpression(pos, tokens);
        REQUIRE(KEY_RIGHT_PARENTHESIS_OPERATION);
        return expr;
    }
    else if (isNumber(CUR()))
    {
        double val = CUR()->value.num;
        (*pos)++;;
        return _NUM(val);
    }
    else if (isIdentifier(CUR()))
    {
        char* name = strdup(CUR()->value.id);
        (*pos)++;;
        return _ID(name);
    }
    else {sintaxError(); return nullptr;}
}
