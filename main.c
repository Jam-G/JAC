#include<stdio.h>
#include"lexical.h"
#include"syntax-analyzer.tab.h"
extern int yyrestart(FILE *);
struct Node *root;
int haserror;
int main(int argc, char **argv){
	if(argc <= 1)
		return -1;
	FILE *f = fopen(argv[1], "r");
	if(!f){
		perror("file not found\n" );
		return -1;
	}
	root = NULL;
	yylineno = 1;
	haserror = 0;
	yyrestart(f);
	yyparse();
	if(haserror == 0){
		printTree(root, 0);
	}
	return 0;
}


