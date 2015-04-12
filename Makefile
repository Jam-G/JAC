all: flex bison parser

.PHONY:all flex bison parser
flex:lexical-analyzer.l
	flex lexical-analyzer.l
bisondebug:syntax-analyzer.y
	bison -d -t -v syntax-analyzer.y
bison:syntax-analyzer.y
	bison -d -t syntax-analyzer.y
parser:main.c syntax-analyzer.tab.c syntax-analyzer.tab.h lexical.c lexical.h lex.yy.c
	gcc main.c syntax-analyzer.tab.c lexical.c -Wall -lfl -ly -o parser
.PHONY:clean debug
clean :
	rm -f lex.yy.c
	rm -f syntax-analyzer.tab.*
	rm -f parser
	rm -f *.output	
debug:flex bisondebug parser


