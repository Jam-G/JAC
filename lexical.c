#include"lexical.h"
#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
#include<malloc.h>
#include<stdarg.h>
struct Node *createNode(int row, enum Type tp, enum TokenType tt, char *nm){
	struct Node * nd = (struct Node *)malloc(sizeof(struct Node));
	nd ->lineno = row;
	nd ->type = tp;
	nd ->tokentype = tt;
	nd ->name = (char *)malloc(strlen(nm) + 1);
	strcpy(nd ->name, nm);
	nd ->name[strlen(nm)] = '\0';//add the end of string in case strcpy did not add
	nd ->brother = NULL;
	nd ->child = NULL;
	return nd;
}

struct Node *destroyNode(struct Node *gb){
	if(gb != NULL){
		if(gb ->name != NULL){
			free(gb ->name);
			gb ->name = NULL;
		}
		free(gb);
	}
	return NULL;
}

struct Node *addChild(struct Node *ft, int cn, ...){
//	printf("enter add :%d\n", cn);
	if(ft == NULL || cn <= 0){
		return NULL;
	}
	va_list ap;
	va_start(ap, cn);
	
	if(ft ->child == NULL){
		ft ->child = va_arg(ap, struct Node*);
		cn --;
	}
	struct Node *addpoint = ft ->child;
	for(; addpoint ->brother != NULL; addpoint = addpoint ->brother);
	for(;cn > 0; cn --){
		addpoint ->brother = va_arg(ap, struct Node*);
		if(addpoint ->brother == NULL){
		//	printf("add NULL:%d\n", cn);
			continue;
		}else{
			addpoint = addpoint ->brother;
		}
	}
//	printf("leave add :%d\n", cn);
	return addpoint;
}
int iatoi(char *str){
	int va;
	if(str == NULL || strlen(str) == 0)
		return 0;
	if(str[0] == '0' && strlen(str) >= 2){
		if(str[1] == 'x' || str[1] == 'X')
			sscanf(str, "%x", &va);
		else sscanf(str, "%o", &va);
	}else return atoi(str);//十进制
	return va;
}
float iatof(char *str){
	float va;
	sscanf(str, "%f", &va);
	return va;
}
struct Node *printTree(struct Node *rt, int nLayer){
	if(rt == NULL){
		return NULL;
	}
	//打印自己
	int k = 0; 
	for(k = 0; k < nLayer; k ++)
		printf("   ");

	if(rt ->type == END){
		printf("%s: ", ttoa(rt ->tokentype));
		if(rt ->tokentype == _INT){
			printf("%d\n", iatoi(rt ->name));
		}else if(rt ->tokentype == _FLOAT){
			printf("%f\n", iatof(rt ->name));
		}else
			printf("%s\n", rt ->name);
	}else{	
		printf("%s  (%d)\n", rt->name, rt ->lineno);
	}	
	//打印每个孩子,递归的打印
	struct Node* current = rt ->child;
	
	for(; current != NULL; current = current ->brother){
			printTree(current, nLayer + 1);
	}
	fflush(stdout);
	return rt;
}

char * ttoa(enum TokenType tt){
	switch(tt){
		case _INT: return "INT";
		case _FLOAT: return "FLOAT";
		case _ID: return "ID";
		case _SEMI: return "SEMI";
		case _COMMA: return "COMMA";
		case _ASSIGNOP: return "ASSIGNOP";
		case _RELOP: return "RELOP";
		case _PLUS: return "PLUS";
		case _MINUS: return "MINUS";
		case _STAR: return "STAR";
		case _DIV: return "DIV";
		case _AND: return "AND";
		case _OR: return "OR";
		case _DOT: return "DOT";
		case _NOT: return "NOT";
		case _TYPE: return "TYPE";
		case _LP: return "LP";
		case _RP: return "RP";
		case _LB: return "LB";
		case _RB: return "RB";
		case _LC: return "LC";
		case _RC: return "RC";
		case _STRUCT: return "STRUCT";
		case _RETURN: return "RETURN";
		case _IF: return "IF";
		case _ELSE: return "ELSE";
		case _WHILE: return "WHILE";
		case _Program: return "Program";
		case _ExtDefList: return "ExtDefList";
		case _ExtDef: return "ExtDef";
		case _ExtDecList: return "ExtDecList";
		case _Specifier: return "Specifier";
		case _StructSpecifier: return "StructSpecifier";
		case _OptTag: return "OptTag";
		case _Tag: return "Tag";
		case _VarDec: return "VarDec";
		case _FunDec: return "FunDec";
		case _VarList: return "VarList";
		case _ParamDec: return "ParamDec";
		case _CompSt: return "Compst";
		case _StmtList: return "StmtList";
		case _Stmt: return "Stmt";
		case _DefList: return "DefList";
		case _Def: return "Def";
		case _DecList: return "DecList";
		case _Dec: return "Dec";
		case _Exp: return "Exp";
		case _Args: return "Args";
	}
	return "ERROR";
}
