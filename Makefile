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
.PHONY:clean debug test testall testac
clean :
	rm -f lex.yy.c
	rm -f syntax-analyzer.tab.*
	rm -f parser
	rm -f *.output	
debug:flex bisondebug parser
test:
	./parser test1.cmm
	./parser test2.cmm
	./parser test3.cmm
	./parser test4.cmm
	./parser mtest1.cmm
	./parser mtest2.cmm
	./parser mtest3.cmm
	./parser mtest4.cmm
	./parser mtest5.cmm
	./parser mtest6.cmm
testall:
	./parser *.cmm	
testac:
	./parser a*.c
