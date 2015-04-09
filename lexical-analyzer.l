/*** Definition section ***/
 
%{
/* C code to be copied verbatim */
#include <stdio.h>
%}
 
DIGIT [0-9]  /*数字*/
NONZERO [1-9] /*非零数字*/
LETTER [a-zA-Z]
LETTER_ [a-zA-Z_] /*字母和下划线*/
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
FLOAT {FLAOT_NOR}|{FLOAT_EXP}

ID {LETTER_}({LETTER_}|{DIGIT})*

%option yylineno 
%%
    /*** Rules section ***/
[ \t]* {/*do nothing space */}
{INTEGER} {yylval = createNode()}
 
%%
/*** C Code section ***/
 
int main(int argc, char** argv)
{
    if(argc > 1){
		if(!(yyin = fopen(argv[1], "r"))){
			perror("the file named %s can't open!\n", argc[1]);
			return -1;
		}
	}
	while(yylex() != 0);
    return 0;
}
