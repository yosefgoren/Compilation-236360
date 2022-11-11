#include "tokens.hpp"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define errorTerm(...) do{printf(__VA_ARGS__); exit(0);}while(0)
#define ILLEGAL_HEX_DIGIT -1

extern int yylex();
const char* escape_seq_error = "Error undefined escape sequence";

static void printToLength(const char* buff, int length){
	//option 1:
	for(int i = 0; i < length; ++i){
		printf("%c", buff[i]);
	}
	//option 2:
	//printf("%.*s", length, buff);
}

#define LENGTH_TO_NULL_END -1
#define STD_OUT 1
static void printTokenFull(int line_num, const char* token_name
	, const char* token_value, int len_of_token_value = LENGTH_TO_NULL_END){
    printf("%d %s ", line_num, token_name);
	if(len_of_token_value != LENGTH_TO_NULL_END){
		//printf("len is: %d\n", len_of_token_value);//DB
		printToLength(token_value, len_of_token_value);
	} else {
		printf("%s", token_value);
	}
	printf("\n");
}

void printToken(const char* token_name){
    printTokenFull(yylineno, token_name, yytext);
}

void illegalChar(){
	//printf("illegalChar: len is: %d, line is: %d, first char num: 0x%x.\n", yyleng, yylineno, *yytext);//DB
	errorTerm("Error %s\n", yytext);
}
void unclosedString(){
	//printf("EUS got: %s.\n", yytext);//DB
	errorTerm("Error unclosed string\n");
}

static int hexDigitToInt(char hex_digit){
	if('0' <= hex_digit && hex_digit <= '9')
		return hex_digit-'0';
	if('A' <= hex_digit && hex_digit <= 'F')
		return hex_digit-'A'+10;

	//for some strage reson... uncapped a-f are not legal.
	// if('a' <= hex_digit && hex_digit <= 'f')
	// 	return hex_digit-'a'+10;
	return ILLEGAL_HEX_DIGIT;
}

static char asciiLiteralToChar(char high_digit, char low_digit){
	int result = hexDigitToInt(high_digit)*16 + hexDigitToInt(low_digit);
	return (char)result;
}

static int isPrintableAsciiNum(int num){
	return (0x20 <= num && num <= 0x7e)
			|| num == 0x09 || num == 0x0a || num == 0x0d;
}

static int isInAsciiLegalRange(char c){
	return 0x00 <= c && c <= 0x7f;
}

static int legalHexSequence(const char* seq){
	if(seq[0] != 'x' && seq[1])
		return 0;
	if(seq[1] == '\0' || seq[2] == '\0')
		return 0;
	if(hexDigitToInt(seq[1]) == ILLEGAL_HEX_DIGIT 
			|| hexDigitToInt(seq[2]) == ILLEGAL_HEX_DIGIT)
		return 0;
	//does the ascii value actually have to be printable?
	return isInAsciiLegalRange(asciiLiteralToChar(seq[1], seq[2]));
}

//the return of this function is the final length of the resulting string
static int formatString(const char* src, char* dst, int length){
	//printf("flex original len: %d.\n", length);//DB
	//make sure the string indeed starts with apostrophes:
	const char* const dst_begining = dst;
	assert(*src == '\"');
	//if it does not end with apostrophes we throw an error
	//, but undef escape seq has higher priority so we check for that.
	
	const char* string_end = src + length - 1;//-1 to ignore the apostrophes at the end of the string.
	src++;//to ignore the parentheses at the start of the string.

	while(src != string_end){
		//printf("got char: %c.\n", *src);//DB
		if(*src == '\n')
			unclosedString();
		//do we need to create an error if the string includes an un-printable char?
		if(*src != '\\'){
			*dst = *src;
		} else{
			src++;
			if(src == string_end)
				unclosedString();
			char escape_sequence[4] = "\0\0\0";
			if(*src == 'x'){
				escape_sequence[0] = 'x';
				for(int i = 1; src+i <= string_end && src[i] != '\"' && i <= 2; i++)
					escape_sequence[i] = src[i];
				if(!legalHexSequence(escape_sequence)){
					errorTerm("%s %s\n", escape_seq_error, escape_sequence);
				}
				char high_digit = *(++src), low_digit = *(++src);
				*dst = asciiLiteralToChar(high_digit, low_digit);
			} else {
				switch(*src){
				case '\\':
					//printf("expected to get here\n");//DB
					*dst = '\\';
					break;
				case '\"':
					*dst = '\"';
					break;
				case 'n':
					*dst = '\n';
					break;
				case 'r':
					*dst = '\r';
					break;
				case 't':
					*dst = '\t';
					break;
				case '0':
					*dst = '\0';
					break;
				default:
					errorTerm("%s %c\n", escape_seq_error, *src);
				}
			}
		}
		src++;
		dst++;
	}
	if(*string_end == '\n')
		unclosedString();
	*dst = '\0';

	return (unsigned long int)(dst)-(unsigned long int)(dst_begining);
}

void printStringToken(){
	//printf("string len is: %d.\n", yyleng);
	char buff[2<<11];
	int result_length = formatString(yytext, buff, yyleng);
	printTokenFull(yylineno, "STRING", buff, result_length);
}

int main(){
	int token;
	while(token = yylex()) {
		switch(token){
			case VOID:
				printToken("VOID");
				break;
    		case INT:
				printToken("INT");
				break;
    		case BYTE:
				printToken("BYTE");
				break;
    		case B:
				printToken("B");
				break;
    		case BOOL:
				printToken("BOOL");
				break;
    		case AND:
				printToken("AND");
				break;
    		case OR:
				printToken("OR");
				break;
    		case NOT:
				printToken("NOT");
				break;
    		case TRUE:
				printToken("TRUE");
				break;
    		case FALSE:
				printToken("FALSE");
				break; 
    		case RETURN:
				printToken("RETURN");
				break; 
    		case IF:
				printToken("IF");
				break; 
    		case ELSE:
				printToken("ELSE");
				break; 
    		case WHILE:
				printToken("WHILE");
				break; 
    		case BREAK:
				printToken("BREAK");
				break; 
    		case CONTINUE:
				printToken("CONTINUE");
				break;
    		case SC:
				printToken("SC");
				break; 
    		case COMMA:
				printToken("COMMA");
				break; 
    		case LPAREN:
				printToken("LPAREN");
				break; 
    		case RPAREN:
				printToken("RPAREN");
				break; 
    		case LBRACE:
				printToken("LBRACE");
				break; 
    		case RBRACE:
				printToken("RBRACE");
				break; 
    		case ASSIGN:
				printToken("ASSIGN");
				break; 
    		case RELOP:
				printToken("RELOP");
				break; 
    		case BINOP:
				printToken("BINOP");
				break; 
    		case COMMENT:
				printTokenFull(yylineno, "COMMENT", "//");
				break; 
    		case ID:
				printToken("ID");
				break; 
    		case NUM:
				printToken("NUM");
				break; 
    		case STRING:
				printStringToken();
				break;
		}
	}
	return 0;
}
