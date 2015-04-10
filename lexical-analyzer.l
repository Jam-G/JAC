/*** Definition section ***/
 
%{
/* C code to be copied verbatim */
#include <stdio.h>
#include"lexical.h"
void * yylval;
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
COMMENT (\/\/.*$)|(\/\*[.\n]*\*\/)
%option yylineno
%option noyywrap



%%
    /*** Rules section ***/
([ \t\n\r])* {/*do nothing space */}
{COMMENT} {printf("line:%d COMMENT:%s\n", yylineno, yytext);}
{INTEGER} {
	yylval = createNode(yylineno, END, INT, yytext);
	printf("line:%d INT:%s\n", yylineno, yytext);
}
{FLOAT} {
	yylval = createNode(yylineno, END, FLOAT, yytext);
	printf("line:%d FLOAT:%s\n", yylineno, yytext);
}
";" {
	yylval = createNode(yylineno, END, SEMI, yytext);
	printf("line:%d SEMI:%s\n", yylineno, yytext);
}
"," {
	yylval = createNode(yylineno, END, COMMA, yytext);
	printf("line:%d COMMA:%s\n", yylineno, yytext);
}
"=="|">="|"<="|"!="|"<"|">" {
	yylval = createNode(yylineno, END, RELOP, yytext);
	printf("line:%d RELOP:%s\n", yylineno, yytext);
}
"=" {
	yylval = createNode(yylineno, END, ASSIGNOP, yytext);
	printf("line:%d ASSIGNOP:%s\n", yylineno, yytext);
}
"+" {
	yylval = createNode(yylineno, END, PLUS, yytext);
	printf("line:%d PLUS:%s\n", yylineno, yytext);
}
"-" {
	yylval = createNode(yylineno, END, MINUS, yytext);
	printf("line:%d MINUS:%s\n", yylineno, yytext);
}
"*" {
	yylval = createNode(yylineno, END, STAR, yytext);
	printf("line:%d STAR:%s\n", yylineno, yytext);
}
"/" {
	yylval = createNode(yylineno, END, DIV, yytext);
	printf("line:%d DIV:%s\n", yylineno, yytext);
}
"&&" {
	yylval = createNode(yylineno, END, AND, yytext);
	printf("line:%d AND:%s\n", yylineno, yytext);
}
"||" {
	yylval = createNode(yylineno, END, OR, yytext);
	printf("line:%d OR:%s\n", yylineno, yytext);
}
"." {
	yylval = createNode(yylineno, END, DOT, yytext);
	printf("line:%d DOT:%s\n", yylineno, yytext);
}
"!" {
	yylval = createNode(yylineno, END, NOT, yytext);
	printf("line:%d NOT:%s\n", yylineno, yytext);
}
{TYPE} {
	yylval = createNode(yylineno, END, TYPE, yytext);
	printf("line:%d TYPE:%s\n", yylineno, yytext);
}
"(" {
	yylval = createNode(yylineno, END, LP, yytext);
	printf("line:%d LP:%s\n", yylineno, yytext);
}
")" {
	yylval = createNode(yylineno, END, RP, yytext);
	printf("line:%d RP:%s\n", yylineno, yytext);

}
"[" {
	yylval = createNode(yylineno, END, LB, yytext);
	printf("line:%d LB:%s\n", yylineno, yytext);
}
"]" {
	yylval = createNode(yylineno, END, RB, yytext);
	printf("line:%d RB:%s\n", yylineno, yytext);
}
"{" {
	yylval = createNode(yylineno, END, LC, yytext);
	printf("line:%d LC:%s\n", yylineno, yytext);
}
"}" {
	yylval = createNode(yylineno, END, RC, yytext);
	printf("line:%d RC:%s\n", yylineno, yytext);
}
"struct" {
	yylval = createNode(yylineno, END, STRUCT, yytext);
	printf("line:%d STRUCT:%s\n", yylineno, yytext);
}
"return" {
	yylval = createNode(yylineno, END, RETURN, yytext);
	printf("line:%d RETURN:%s\n", yylineno, yytext);
}
"if" {
	yylval = createNode(yylineno, END, IF, yytext);
	printf("line:%d IF:%s\n", yylineno, yytext);
}
"else" {
	yylval = createNode(yylineno, END, ELSE, yytext);
	printf("line:%d ELSE:%s\n", yylineno, yytext);
}
"while" {
	yylval = createNode(yylineno, END, WHILE, yytext);
	printf("line:%d WHILE:%s\n", yylineno, yytext);
}
{ID} {
	yylval = createNode(yylineno, END, ID, yytext);
	printf("line:%d ID:%s\n", yylineno, yytext);
}
. {
	printf("Error type A at line %d: Mysterious character %s\n", yylineno, yytext);
}
%%
/*** C Code section ***/

int main(int argc, char** argv)
{
 	if (argc <= 1) return 1;
	FILE* f = fopen(argv[1],"r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}	
	printf("now start\n");
	yyin = f;
	yylex();
	printf("now end\n");
	return 0;
}
