/*** Definition section ***/
 
%{
/* C code to be copied verbatim */
#include <stdio.h>
#include"lexical.h"
#include"syntax-analyzer.tab.h"
int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;\
	yylloc.first_column = yycolumn;\
	yylloc.last_column = yycolumn + yyleng - 1;\
	yycolumn += yyleng;
%}
 
DIGIT [0-9]
NONZERO [1-9] 
LETTER [a-zA-Z]
LETTER_ [a-zA-Z_]
INTEGER_D ({NONZERO}{DIGIT}*)|0
DIGIT_H [0-9a-fA-F]
NONZERO_H [1-9a-fA-F]
INTEGER_H (0x|0X)(({NONZERO_H}{DIGIT_H}*)|0)
DIGIT_O [0-7]
NONZERO_O [1-7]
INTEGER_O 0{NONZERO_O}{DIGIT_O}*
INTEGER {INTEGER_D}|{INTEGER_H}|{INTEGER_O}
FLOAT_NOR {INTEGER_D}\.{DIGIT}+
FLOAT_EXP (({DIGIT}*\.{DIGIT}+)|({DIGIT}+\.{DIGIT}*))[eE][+-]?{DIGIT}+
FLOAT {FLOAT_NOR}|{FLOAT_EXP}
ID {LETTER_}({LETTER_}|{DIGIT})*
TYPE (int)|(float)
COMMENT (\/\/.*)|(\/\*([^*]|[\r\n]|(\*+([^*\/]|[\r\n])))*\*+\/)
%option yylineno
%option noyywrap



%%
    /*** Rules section ***/
{COMMENT} {/*printf("line:%d COMMENT:%s\n", yylineno, yytext);*/}
\n {yycolumn = 1;}
([ \t\r])+ {/*do nothing space */}
{INTEGER} {
	yylval.type_node = createNode(yylineno, END, _INT, yytext);
//	printf("line:%d INT:%s\n", yylineno, yytext);
	return INT;
}
{FLOAT} {
	yylval.type_node = createNode(yylineno, END, _FLOAT, yytext);
//	printf("line:%d FLOAT:%s\n", yylineno, yytext);
	return FLOAT;
}
";" {
	yylval.type_node = createNode(yylineno, END, _SEMI, yytext);
//	printf("line:%d SEMI:%s\n", yylineno, yytext);
	return SEMI;
}
"," {
	yylval.type_node = createNode(yylineno, END, _COMMA, yytext);
//	printf("line:%d COMMA:%s\n", yylineno, yytext);
	return COMMA;
}
"=="|">="|"<="|"!="|"<"|">" {
	yylval.type_node = createNode(yylineno, END, _RELOP, yytext);
//	printf("line:%d RELOP:%s\n", yylineno, yytext);
	return RELOP;
}
"=" {
	yylval.type_node = createNode(yylineno, END, _ASSIGNOP, yytext);
//	printf("line:%d ASSIGNOP:%s\n", yylineno, yytext);
	return ASSIGNOP;
}
"+" {
	yylval.type_node = createNode(yylineno, END, _PLUS, yytext);
//	printf("line:%d PLUS:%s\n", yylineno, yytext);
	return PLUS;
}
"-" {
	yylval.type_node = createNode(yylineno, END, _MINUS, yytext);
//	printf("line:%d MINUS:%s\n", yylineno, yytext);
	return MINUS;
}
"*" {
	yylval.type_node = createNode(yylineno, END, _STAR, yytext);
//	printf("line:%d STAR:%s\n", yylineno, yytext);
	return STAR;
}
"/" {
	yylval.type_node = createNode(yylineno, END, _DIV, yytext);
//	printf("line:%d DIV:%s\n", yylineno, yytext);
	return DIV;
}
"&&" {
	yylval.type_node = createNode(yylineno, END, _AND, yytext);
//	printf("line:%d AND:%s\n", yylineno, yytext);
	return AND;
}
"||" {
	yylval.type_node = createNode(yylineno, END, _OR, yytext);
//	printf("line:%d OR:%s\n", yylineno, yytext);
	return OR;
}
"." {
	yylval.type_node = createNode(yylineno, END, _DOT, yytext);
//	printf("line:%d DOT:%s\n", yylineno, yytext);
	return DOT;
}
"!" {
	yylval.type_node = createNode(yylineno, END, _NOT, yytext);
//	printf("line:%d NOT:%s\n", yylineno, yytext);
	return NOT;
}
{TYPE} {
	yylval.type_node = createNode(yylineno, END, _TYPE, yytext);
//	printf("line:%d TYPE:%s\n", yylineno, yytext);
	return TYPE;
}
"(" {
	yylval.type_node = createNode(yylineno, END, _LP, yytext);
//	printf("line:%d LP:%s\n", yylineno, yytext);
	return LP;
}
")" {
	yylval.type_node = createNode(yylineno, END, _RP, yytext);
//	printf("line:%d RP:%s\n", yylineno, yytext);
	return RP;
}	
"[" {
	yylval.type_node = createNode(yylineno, END, _LB, yytext);
//	printf("line:%d LB:%s\n", yylineno, yytext);
	return LB;
}
"]" {
	yylval.type_node = createNode(yylineno, END, _RB, yytext);
//	printf("line:%d RB:%s\n", yylineno, yytext);
	return RB;
}
"{" {
	yylval.type_node = createNode(yylineno, END, _LC, yytext);
//	printf("line:%d LC:%s\n", yylineno, yytext);
	return LC;
}
"}" {
	yylval.type_node = createNode(yylineno, END, _RC, yytext);
//	printf("line:%d RC:%s\n", yylineno, yytext);
	return RC;
}
"struct" {
	yylval.type_node = createNode(yylineno, END, _STRUCT, yytext);
//	printf("line:%d STRUCT:%s\n", yylineno, yytext);
	return STRUCT;
}
"return" {
	yylval.type_node = createNode(yylineno, END, _RETURN, yytext);
//	printf("line:%d RETURN:%s\n", yylineno, yytext);
	return RETURN;
}
"if" {
	yylval.type_node = createNode(yylineno, END, _IF, yytext);
//	printf("line:%d IF:%s\n", yylineno, yytext);
	return IF;
}
"else" {
	yylval.type_node = createNode(yylineno, END, _ELSE, yytext);
//	printf("line:%d ELSE:%s\n", yylineno, yytext);
	return ELSE;
}
"while" {
	yylval.type_node = createNode(yylineno, END, _WHILE, yytext);
//	printf("line:%d WHILE:%s\n", yylineno, yytext);
	return WHILE;
}
{ID} {
	yylval.type_node = createNode(yylineno, END, _ID, yytext);
//	printf("line:%d ID:%s\n", yylineno, yytext);
	return ID;
}
. {
	printf("Error type A at line %d: Mysterious character %s\n", yylineno, yytext);
	haserror = 1;//then find error type A.need not print tree;
}
%%
/*** C Code section ***/


