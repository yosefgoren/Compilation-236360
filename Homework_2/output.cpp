#include <iostream>
#include "output.hpp"
#include <sstream>

const std::string output::rules[] = {
    /* 1 */ "Program -> Funcs",
    /* 2 */ "Funcs -> epsilon",
    /* 3 */ "Funcs -> FuncDecl Funcs",
    /* 4 */ "FuncDecl -> RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE",
    /* 5 */ "RetType -> Type",
    /* 6 */ "RetType ->  VOID",
    /* 7 */ "Formals -> epsilon",
    /* 8 */ "Formals -> FormalsList",
    /* 9 */ "FormalsList -> FormalDecl",
    /* 10 */ "FormalsList -> FormalDecl COMMA FormalsList",
    /* 11 */ "FormalDecl -> Type ID",
    /* 12 */ "Statements -> Statement",
    /* 13 */ "Statements -> Statements Statement",
    /* 14 */ "Statement -> LBRACE Statements RBRACE",
    /* 15 */ "Statement -> TypeAnnotation Type ID SC",
    /* 16 */ "Statement -> TypeAnnotation Type ID ASSIGN Exp SC",
    /* 17 */ "Statement -> ID ASSIGN Exp SC",
    /* 18 */ "Statement -> Call SC",
    /* 19 */ "Statement -> RETURN SC",
    /* 20 */ "Statement -> RETURN Exp SC",
    /* 21 */ "Statement -> IF LPAREN Exp RPAREN Statement",
    /* 22 */ "Statement -> IF LPAREN Exp RPAREN Statement ELSE Statement",
    /* 23 */ "Statement -> WHILE LPAREN Exp RPAREN Statement",
    /* 24 */ "Statement -> BREAK SC",
    /* 25 */ "Statement -> CONTINUE SC",
    /* 26 */ "Call -> ID LPAREN ExpList RPAREN",
    /* 27 */ "Call -> ID LPAREN RPAREN",
    /* 28 */ "ExpList -> Exp",
    /* 29 */ "ExpList -> Exp COMMA ExpList",
    /* 30 */ "Type -> INT",
    /* 31 */ "Type -> BYTE",
    /* 32 */ "Type -> BOOL",
    /* 33 */ "TypeAnnotation -> epsilon",
    /* 34 */ "TypeAnnotation -> CONST",
    /* 35 */ "Exp -> LPAREN Exp RPAREN",
    /* 36 */ "Exp -> Exp BINOP Exp",
    /* 37 */ "Exp -> ID",
    /* 38 */ "Exp -> Call",
    /* 39 */ "Exp -> NUM",
    /* 40 */ "Exp -> NUM B",
    /* 41 */ "Exp -> STRING",
    /* 42 */ "Exp -> TRUE",
    /* 43 */ "Exp -> FALSE",
    /* 44 */ "Exp -> NOT Exp",
    /* 45 */ "Exp -> Exp AND Exp",
    /* 46 */ "Exp -> Exp OR Exp",
    /* 47 */ "Exp -> Exp RELOP Exp",
    /* 48 */ "Exp -> LPAREN TypeAnnotation Type RPAREN Exp"
};

void output::printProductionRule(const int ruleno) {
    std::cout << ruleno << ": " << output::rules[ruleno-1] << "\n";
}

void output::errorLex(const int lineno) {
    std::cout << "line " << lineno << ": lexical error\n";
}

void output::errorSyn(const int lineno) {
    std::cout << "line " << lineno << ": syntax error\n";
}
