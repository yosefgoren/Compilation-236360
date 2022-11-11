%{
#include "tokens.hpp"
#include <stdlib.h>
void illegalChar();
void unclosedString();
//(\"([^\"\n]|(\\\"))*\")
%}

%option noyywrap
%option yylineno

real_operation				((==)|(!=)|(<=)|(>=)|<|>)
binary_operation 			[\+-\/\*]
comment						(\/\/[^\n\r]*)
identifier					([a-zA-Z][a-zA-Z0-9_]*)
natural_number				(0|(-?[1-9][0-9]*))
whitespace  				([\t\n\r ])

full_string					(\"(\\\")?(([^\\\"\n](\\\\)*\\\")|[^\"\n])*\")
unclosed_string				(\"[^\"]*\n)

printable_ascii_literal		(\\x([2-6][0-9a-fA-f]|7[0-9a-eA-E]))
escape_squance				((\\\\)|(\\\")|(\\n)|(\\r)|(\\t)|(\\0)|{printable_ascii})
legal_string_atom			([^\\\"\n\r]|{escape_squance})
legal_string				(\"{legal_string_atom}*\")

%%
void				return VOID;
int					return INT;
byte				return BYTE;
b					return B;
bool				return BOOL;
and					return AND;
or					return OR;
not					return NOT;
true				return TRUE;
false				return FALSE;
return				return RETURN;
if					return IF;
else				return ELSE;
while				return WHILE;
break				return BREAK;
continue			return CONTINUE;
;					return SC;
,					return COMMA;
\(					return LPAREN;
\)					return RPAREN;
\{					return LBRACE;
\}					return RBRACE;
=					return ASSIGN;
{real_operation}	return RELOP;
{binary_operation}	return BINOP;
{comment}			return COMMENT;
{identifier}		return ID;
{natural_number}	return NUM;
{full_string}		return STRING;
{unclosed_string}	unclosedString();
{whitespace}
.       			illegalChar();
%%
