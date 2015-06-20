#include<stdio.h>
#include"lexical.h"
#include"syntax-analyzer.tab.h"
#include"semantic.h"
#include"intercode.h"
#include<string.h>
extern int yyrestart(FILE *);
struct Node *root;
int haserror;
InterCodes irtable = NULL;
extern void print_IRs(char *filename);
int gen_ir(struct Node * node , int level){
	InterCodes icode;
	//if(level == 0) printf("------------------------\n");
	if(node == NULL) return 0;
	else if(node->tokentype == _Exp){
		icode = translate_Exp(node, NULL);
		irtable = link_ir(irtable, icode);
		return 0;
	}
	else if(node->tokentype == _Stmt){
		icode = translate_Stmt(node);
		irtable = link_ir(irtable, icode);
		return 0;
	}
	else if(node->tokentype == _ExtDef){
		icode = translate_Extdef(node);
		irtable = link_ir(irtable, icode);
		return 0;
	}
	return 1;
}

void traverse(int (*func)(struct Node *, int), struct Node * root, int level) {
	//Node is not empty
	if ((*func)(root, level) == 1) {
		if (root->child != NULL)
			traverse(func, root->child, level+1);
    		if (root->brother != NULL)
			traverse(func, root->brother, level);
	}
	//Node is empty, so has no children
	else {
		if (root->brother != NULL)
			traverse(func, root->brother, level);
	}
}

int main(int argc, char **argv){
	if(argc <= 1)
		return -1;
	int i = 1;
	if(argc == 2){
//		printf("\n\ntesting:%s\n", argv[i]);
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
		//	printTree(root, 0);
			semantic();
			printf("\n");
		}
		destroyTree(root);
		close(f);
	}
	if(argc == 3){
//		printf("\n\ntesting:%s\n", argv[i]);
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
		//	printTree(root, 0);
			semantic();
			traverse(&gen_ir, root, 0);
			optimize_ir(&irtable);
			optimize_ir(&irtable);
			optimize_ir(&irtable);
	//		output_ir(irtable, argv[]);
			print_IRs(argv[2]);
		//output_read_write();
		}
		destroyTree(root);
		close(f);
	}
	if(argc == 4){
//		printf("\n\ntesting:%s\n", argv[i]);
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
		//	printTree(root, 0);
			semantic();
			traverse(&gen_ir, root, 0);
			optimize_ir(&irtable);
			optimize_ir(&irtable);
			optimize_ir(&irtable);
			output_ir(irtable, argv[2]);
			print_IRs(argv[3]);
		//output_read_write();
		}
		destroyTree(root);
		close(f);
	}
	return 0;
}


