#include<stdio.h>
#include"lexical.h"
#include"syntax-analyzer.tab.h"
#include"semantic.h"
extern int yyrestart(FILE *);
struct Node *root;
int haserror;
int main(int argc, char **argv){
	if(argc <= 1)
		return -1;
	int i = 1;
	for(i = 1; i < argc; i ++){
		printf("\n\ntesting:%s\n", argv[i]);
		FILE *f = fopen(argv[i], "r");
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
			semantic();
		}
		destroyTree(root);
		close(f);
	}
	return 0;
}


