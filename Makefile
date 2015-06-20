all: flex bison parser

.PHONY:all flex bison parser
flex:lexical-analyzer.l
	flex lexical-analyzer.l
bisondebug:syntax-analyzer.y
	bison -d -t -v syntax-analyzer.y
bison:syntax-analyzer.y
	bison -d -t syntax-analyzer.y
parser:main.c syntax-analyzer.tab.c syntax-analyzer.tab.h lexical.c lexical.h lex.yy.c semantic.h semantic.c intercode.c intercode.h mips.c
	gcc main.c syntax-analyzer.tab.c lexical.c semantic.h semantic.c intercode.c mips.c -Wall -lfl -ly -o parser
.PHONY:clean debug test testall testac
clean :
	rm -f lex.yy.c
	rm -f syntax-analyzer.tab.*
	rm -f parser
	rm -f *.output	
debug:
	gcc main.c syntax-analyzer.tab.c lexical.c semantic.h semantic.c intercode.c mips.c -Wall -lfl -ly -g -o parser

