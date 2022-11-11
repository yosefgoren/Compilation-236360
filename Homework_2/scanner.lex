%{
	#include "output.hpp"
	#include "parser.tab.hpp"
%}

%option noyywrap
%option yylineno

real_operation		(==)|(!=)|(<)|(>)|(<=)|(>=)
binary_operation	[(\+)(\-)(\*)(/)]
number				(0|[1-9][0-9]*)
string				(\"([^\n\r\"\\]|\\[rnt\"\\])+\")
comment				(\/\/[^\r\n]*[\r\n(\r\n)])
whitespace			[\t\r\n ]

%%
void							return VOID;
int								return INT;
byte							return BYTE;
b								return B;
bool							return BOOL;
const							return CONST;
and								return AND;
or								return OR;
not								return NOT;
true							return TRUE;
false							return FALSE;
return							return RETURN;
if								return IF;
else							return ELSE;
while							return WHILE;
break							return BREAK;
continue						return CONTINUE;
;								return SC;
,								return COMMA;
\(								return LPAREN;
\)								return RPAREN;
\{								return LBRACE;
\}								return RBRACE;
=								return ASSIGN;
{real_operation}				return RELOP;
{binary_operation}				return BINOP;
[a-zA-Z][a-zA-Z0-9]*			return ID;
{number}						return NUM;
{string}						return STRING;
{comment}						;
{whitespace}					;
.								{output::errorLex(yylineno); exit(420);}

%%