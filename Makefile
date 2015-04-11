all: flex bison parser

.PHONY:all flex bison parser
flex:lexical-analyzer.l
	flex lexical-analyzer.l
bison:syntax-analyzer.y
	bison -d -t -v syntax-analyzer.y
parser:main.c syntax-analyzer.tab.c syntax-analyzer.tab.h lexical.c lexical.h lex.yy.c
	gcc main.c syntax-analyzer.tab.c lexical.c -Wall -lfl -ly -o parser
.PHONY:clean 
clean : 
	rm -rf *.o
	rm -rf lex.yy.c
	rm -rf syntax-analyzer.tab.*
	rm parser
