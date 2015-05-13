#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"semantic.h"
#include"lexical.h"
#include<assert.h>
#define HASHLEN 1000
symnode_t *symtable[HASHLEN];
strnode_t *strtable[HASHLEN];
funnode_t *funtable[HASHLEN];

void semantic(){
	inittables();
	semantic_program(root);
}

void inittables()
{
	memset(symtable, 0, sizeof(symnode_t *) * HASHLEN);
	memset(strtable, 0, sizeof(strnode_t *) * HASHLEN);
	memset(funtable, 0, sizeof(funnode_t *) * HASHLEN);
}
void semantic_program(struct Node *node){
	if(node == NULL || node->tokentype != _Program){
		printf("the node is not a program\n");
		return;
	}
	struct Node *child = node->child;
	//the child must be a ExtDefList;,so just call ExtDefList anysis;
	semantic_ExtDefList(child);
}
void semantic_ExtDefList(struct Node *node){
	if(node == NULL || node->tokentype != _ExtDefList){
		printf("the node is not a ExtDefList\n");
		return;
	}
	struct Node *child = node->child;
	while(child != NULL){
		if(child->tokentype == _ExtDef){
			semantic_ExtDef(child);
		}else if(child->tokentype == _ExtDefList){
			semantic_ExtDefList(child);
		}
		child = child->brother;
	}
}

void semantic_ExtDef(struct Node *node){
	if(node == NULL || node->tokentype != _ExtDef){
		printf("the node is not a ExtDef\n");
		return;
	}
	struct Node *child = node->child;
	enum VarType type;
	struct Structure * strp = semantic_Specifier(child, &type);
	if(type == V_ERROR){
		return;
	}
	child = child->brother;
	if(child->tokentype == _ExtDecList){
		semantic_ExtDecList(child, type, strp);
	}else if(child->tokentype == _SEMI){
		//encounter ;, need to do nothing, 
		;
	}else if(child->tokentype == _FunDec){
		semantic_FunDec(child, type, strp);
		child = child->brother;
		semantic_CompSt(child);
	}
}

struct Structure * semantic_Specifier(struct Node *node, enum VarType *type){
	if(node == NULL || node->tokentype != _Specifier){
		printf("the node is not a Specifier\n");
		*type = V_ERROR;
		return NULL;
	}
	struct Node *child = node ->child;
	while(child != NULL){
		switch(child->tokentype){
			case _TYPE:{
						   if(strcmp(child->name, "int") == 0)
							  *type = V_INT;
						   else if(strcmp(child->name, "float") == 0)
							   *type = V_FLOAT;
						   else
							   *type = V_ERROR;
						   return NULL;
					   }
			case _StructSpecifier:{
									  return semantic_StructSpecifier(child, type);
								  }
			default:{
						*type = V_ERROR;
						return NULL;
					}
		}
	}
	*type = V_ERROR;
	return NULL;
}

struct Structure * semantic_StructSpecifier(struct Node *node, enum VarType *type){
	if(node == NULL || node->tokentype ){
		printf("the node is not a StructSpecifier\n");
		*type = V_ERROR;
		return NULL;
	}
	struct Node *child = node->child;
	child = child->brother;//the first must be struct,then can skip it
	if(child ->tokentype == _Tag){//this is the id of struct name, dont have to anysis the structure
		*type = V_STRUCT;
		return getStructureByName(child->child->name);
	}else if(child ->tokentype == _OptTag){
		*type = V_STRUCT;
		//child is opttag, child's child is ID OR NULL, so pass the id node ,
		//then can check if id node is NULL
		return newStruct(child->child, child->brother->brother);
	}
	*type = V_ERROR;
	return NULL;
}

void semantic_ExtDecList(struct Node *node, enum VarType basetype, struct Structure * strp){
	if(node == NULL || node->tokentype != _ExtDecList){
		printf("the node is not a ExtDecList\n");
		return;
	}
	struct Node *child = node->child;	
	semantic_VarDec(child, basetype, strp);
	child = child->brother;
	if(child != NULL)
		semantic_ExtDecList(child->brother, basetype, strp);//递归
}

struct VariMsg * semantic_VarDec(struct Node *node, enum VarType basetype, struct Structure *strp){
	if(node == NULL || node->tokentype != _VarDec){
		printf("the node is not a VarDec\n");
		return NULL;
	}
	struct Node *child = node->child;
	if(child->tokentype == _ID){
		union Varp stp;
		stp.sp = strp;
		return newVar(child, basetype, stp);
	}else if(child->tokentype == _VarDec){	
		union Varp arraybase;
		arraybase.ap = malloc(sizeof(struct ArrayMsg));	
		arraybase.ap->basetype = basetype;
		arraybase.ap->base.sp = strp;
		arraybase.ap->size = getIntVal(child->brother->brother);
		return newArrayVar(child, arraybase);
	}
}

void semantic_FunDec(struct Node *node, enum VarType retype, struct Structure *strp){
	if(node == NULL || node->tokentype != _FunDec){
		printf("the node is not a FunDec\n");
		return;
	}
	struct Node *child = node->child;
	struct ParList * args;
	if(child->brother->brother->tokentype == _VarList){
		args = semantic_VarList(child->brother->brother);
	}else{
		args = NULL;// means this function's args is NULL
	}
	newFunction(child, retype, strp, args);
}

struct ParList* semantic_VarList(struct Node *node){
	if(node == NULL || node->tokentype != _VarList){
		printf("the node is not a VarList\n");
		return NULL;
	}
	struct Node *child = node->child;
	struct ParList *head = malloc(sizeof(struct ParList));
	semantic_ParamDec(child,&(head->partype), &(head->parpointer), &(head->name));
	head->next = NULL;
	child = child->brother;
	if(child != NULL){
		head->next = semantic_VarList(child->brother);
	}
}

void semantic_ParamDec(struct Node *node, enum VarType *type, union Varp *vp, char **name){
	if(node == NULL || node->tokentype != _ParamDec){
		printf("the node is not ParamDec\n");
		*type = V_ERROR;
		return;
	}
	struct Node *child = node->child;
	enum VarType spectype;
	struct Structure * strp = semantic_Specifier(child, &spectype);
	*type = spectype;
	(*vp).sp = strp;
	if(spectype == V_ERROR)
		return;
	child = child->brother;
	struct VariMsg * varp = semantic_VarDec(child, spectype, strp);
	*name = varp->name;
	assert(varp->type == spectype);
}


void semantic_CompSt(struct Node *node){
	if(node == NULL || node->tokentype != _CompSt){
		printf("the node is not CompSt\n");
		return;
	}
	struct Node *child = node->child;
	child = child->brother;
	semantic_DefList(child);
	child = child->brother;
	semantic_StmtList(child);
}

void semantic_DefList(struct Node *node){
	if(node == NULL || node->tokentype != _DefList){
		printf("the node is not a DefList\n");
		return;
	}
	struct Node *child = node->child;
	semantic_Def(child);
	child = child->brother;
	if(child != NULL)
		semantic_DefList(child);
}

void semantic_StmtList(struct Node *node){
	if(node == NULL || node->tokentype != _StmtList){
		printf("the node is not a StmtList\n");
		return;
	}
	struct Node *child = node->child;
	semantic_Stmt(child);
	child = child->brother;
	if(child != NULL)
		semantic_StmtList(child);
}

void semantic_Stmt(struct Node *node){
	if(node == NULL || node->tokentype != _Stmt){
		printf("the node is not a Stmt\n");
		return;
	}
	struct Node *child = node->child;

}
void semantic_Def(struct Node *node){
	if(node != NULL || node->tokentype != _Def){
		printf("the node is not a Def\n");
		return;
	}
	struct Node *child = node->child;
	enum VarType type;
	struct Structure *strp = semantic_Specifier(child, &type);
	if(type == V_ERROR)
		return;
	child = child->brother;
	semantic_DecList(child, type, strp);
}

void semantic_DecList(struct Node *node, enum VarType basetype, struct Structure * strp){
	if(node == NULL || node->tokentype != _DecList){
		printf("the node is not a DecList\n");
		return;
	}
	struct Node *child = node->child;	
	semantic_Dec(child, basetype, strp);
	child = child->brother;
	if(child != NULL)
		semantic_DecList(child->brother, basetype, strp);//递归
}


void semantic_Dec(struct Node *node, enum VarType basetype, struct Structure *strp){
	if(node == NULL || node->tokentype != _Dec){
		printf("the node is not a Dec\n");
		return;
	}
	struct Node *child = node->child;
	struct VariMsg * left = semantic_VarDec(child, basetype, strp);
	child = child->brother;
	enum VarType exptype;
	union Varp vp;
	if(child != NULL){
		child = child->brother;
		int c ;
		vp = semantic_Exp(&c, child, &exptype);	
		if(checkAssignType(left->type, left->tp, exptype, vp) < 0){
			printf("Error type 5 at Line %d:left and right type can't match\n", child->lineno);
		}
	}
	
}

union Varp semantic_Exp(int *leftorright, struct Node *node, enum VarType *type){
	if(node == NULL || node->tokentype != _Exp){
		printf("the node is not a Exp\n");
		*type = V_ERROR;
		union Varp t;
		t.ap = NULL;
		return t;
	}
	struct Node *child = node->child;
	if(child->tokentype == _Exp){
		int left;
		enum VarType etp;
		union Varp et = semantic_Exp(&left, child, &etp);
		child = child->brother;
		if(child->tokentype == _ASSIGNOP){
			if(left < 0)
				printf("Error type 6 at Line %d:the left exp only have right val\n", child->lineno);
			child = child->brother;
			enum VarType rightetp;
			union Varp rightrt = semantic_Exp(&left, child, &rightetp);
			if(checkAssignType(etp, et, rightetp, rightrt) < 0){
				printf("Error type 5 at Line %d:left and right type can't match\n", child->lineno);
			}
			*leftorright = -1;//assign exp dont have left value
			*type = etp;
			return et;
		}else if(child->tokentype == _LB){//array 
			if(etp != V_ARRAY){
				printf("Error type 10 at Line %d:the exp is not an array\n", child->lineno);
			}
			child = child->brother;
			enum VarType indextype;
			semantic_Exp(&left, child, &indextype);
			if(indextype != _INT){
				printf("Error type 12 at Line %d:the array's index is not an int\n", child->lineno);
			}
			*leftorright = 1;//array has left value
			if(etp == V_ARRAY){
				*type = et.ap->basetype;
			}
			else {
				*type = etp;
			}
			return et.ap->base;
		}else if(child->tokentype == _DOT){//struct

		}else{//op
		
		}
	}else if(child->tokentype == _LP){
	
	}else if(child->tokentype == _ID){
	
	}else if(child->tokentype == _MINUS){
	
	}else if(child->tokentype == _NOT){
	}else if(child->tokentype == _INT){
	}else if(child->tokentype == _FLOAT){
	}
	
}


