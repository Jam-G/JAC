#include<stdio.h>
#include"lexical.h"
struct Node *root;
int error;
int main(int argc, char **argv){
	if(argc <= 1)
		return -1;
	FILE *f = fopen(argv[1], "r");
	if(!f){
		perror("no file :%s\n", argv[1]);
		retrun -1;
	}
	root = NULL;
	yylineno = 1;
	error = 0;
	yyrestart(f);
	yyparse();
	if(error == 0){
		printTree(root, 0);
	}
}


