#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"semantic.h"
#include"lexical.h"
#include<assert.h>
#define HASHLEN 1000
#define STACKLEN 100

int funarg = 0;
symnode_t *symtable[HASHLEN];
strnode_t *strtable[HASHLEN];
funnode_t *funtable[HASHLEN];
int vaCount = 0;
struct FuncMsg * nowFunc;
int nowerror = 0;
int structlayer = 0; 
//作用域嵌套
symnode_t* symstack[STACKLEN];
symnode_t* currenttail;//当前作用域的尾节点
int stackindex = 0;

void add_read_write(){
	struct Node node;
	node.name = "read";
	node.lineno = 0;
	node.type = NOTEND;
	node.tokentype = _ID;
	node.child = NULL;
	node.brother = NULL;
	enum VarType retype;
	retype = V_INT;
	newFunction(&node, retype, NULL, NULL, 1);
	node.name = "write";
	struct ParList *p = (struct ParList *)malloc(sizeof(struct ParList));
	p->next = NULL;
	p->partype = V_INT;	
	newFunction(&node, retype, NULL, p, 1);	
}
void semantic(){
	inittables();
	add_read_write();
	semantic_program(root);
	checkNoDeFun();//最后检查只有声明没有定义的函数
}
void checkNoDeFun(){
	int i = 0;
	for(i = 0; i < HASHLEN; i ++){
		funnode_t * temp = funtable[i];
		while(temp != NULL){
			if(temp->fun->dord < 0){
				printf("Error type 18 at Line %d:undefined function \"%s\"\n", 0 - temp->fun->dord, temp->fun->name);
			}
			temp = temp->next;
		}
	}
}

void inittables()
{
	memset(symtable, 0, sizeof(symnode_t *) * HASHLEN);
	memset(strtable, 0, sizeof(strnode_t *) * HASHLEN);
	memset(funtable, 0, sizeof(funnode_t *) * HASHLEN);
	nowFunc = NULL;
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
//	printf("ExtDefList\n");
	if(child != NULL){
		semantic_ExtDef(child);
//		printf("exit of ExtDef\n");
		child = child->brother;
		if(child != NULL){
//			printf("next ExtDefList\n");
			semantic_ExtDefList(child);
		}
		
	}
}

void semantic_ExtDef(struct Node *node){
	if(node == NULL || node->tokentype != _ExtDef){
		printf("the node is not a ExtDef\n");
		return;
	}
	struct Node *child = node->child;
	enum VarType type;
	if(child->brother->tokentype == _FunDec){
		stackindex ++;
		symstack[stackindex] = NULL;
	}
	struct Structure * strp = semantic_Specifier(child, &type);
//	printf("anaysis a specifier @layer %d, type %d, struct %d, int %d\n",stackindex,  type, V_STRUCT, V_INT);
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
		if(child->brother->tokentype == _CompSt)//函数定义
			semantic_FunDec(child, type, strp, child->lineno);
		else//函数声明
			semantic_FunDec(child, type, strp, 0 - child->lineno);
		child = child->brother;
		if(child ->tokentype == _CompSt)
			semantic_CompSt(child);
		symnode_t *temp = symstack[stackindex];
		while(temp != NULL){
			temp->layer = -1;
			temp = temp->layernext;
		}
		stackindex --;
	}
}

struct Structure * semantic_Specifier(struct Node *node, enum VarType *type){
	if(node == NULL || node->tokentype != _Specifier){
		printf("the node is not a Specifier\n");
		if(node != NULL)
			printf("the node is %s\n", node->name);
		*type = V_ERROR;
		return NULL;
	}
	struct Node *child = node ->child;
	if(child != NULL){
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
	if(node == NULL || node->tokentype != _StructSpecifier){
		printf("the node is not a StructSpecifier\n");
		if(node != NULL)
			printf("the node is %s\n", node->name);
		*type = V_ERROR;
		return NULL;
	}
	struct Node *child = node->child;
	child = child->brother;//the first must be struct,then can skip it
	if(child ->tokentype == _Tag){//this is the id of struct name, dont have to anysis the structure
		*type = V_STRUCT;
	//	printf("want to get struct\n");
		struct Structure * r = getStructureByName(child->child->name);
		if(r == NULL)
			printf("Error type 17 at Line %d:undefined structure\n", node->lineno);
		return r;
	}else if(child ->tokentype == _OptTag){
		*type = V_STRUCT;
		//child is opttag, child's child is ID OR NULL, so pass the id node ,
		//then can check if id node is NULL
//		printf("try to new a struct\n");
		return newStruct(child->child, child->brother->brother);
	}
	*type = V_ERROR;
	return NULL;
}

void semantic_ExtDecList(struct Node *node, enum VarType basetype, struct Structure * strp){
	if(node == NULL || node->tokentype != _ExtDecList){
		printf("the node is not a ExtDecList\n");
		if(node != NULL)
			printf("the node is%s\n", node->name);
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
	return NULL;
}

void semantic_FunDec(struct Node *node, enum VarType retype, struct Structure *strp, int dord){
	if(node == NULL || node->tokentype != _FunDec){
		printf("the node is not a FunDec\n");
		return;
	}
	struct Node *child = node->child;
	struct ParList * args;
	if(child->brother->brother->tokentype == _VarList){
		funarg = 1;
		args = semantic_VarList(child->brother->brother);
		funarg = 0;
	}else{
		args = NULL;// means this function's args is NULL
	}
	if(nowerror == 1){
		//险释放上一哥错误函数的空间
		free(nowFunc);
		nowerror = 0;//恢复无错情况
	}
	nowFunc = newFunction(child, retype, strp, args, dord);
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
	return head;
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
//	if(varp->type != spectype){
//		printf("varp type is %d, spectype %d\n", varp->type, spectype);
//	}
//	assert(varp->type == spectype);
}


void semantic_CompSt(struct Node *node){
	if(node == NULL || node->tokentype != _CompSt){
		printf("the node is not CompSt\n");
		return;
	}
	struct Node *child = node->child;
	child = child->brother;
	if(child != NULL && child->tokentype == _DefList){
		semantic_DefList(child);
		child = child->brother;
	}
	if(child != NULL && child->tokentype != _RC)
		semantic_StmtList(child);
}

void semantic_DefList(struct Node *node){
	if(node == NULL || node->tokentype != _DefList){
		printf("the node is not a DefList\n");
		if(node != NULL)
			printf("the node is %s\n", node->name);
		return;
	}
	struct Node *child = node->child;
	if(child != NULL){
		semantic_Def(child);
		child = child->brother;
		if(child != NULL)
			semantic_DefList(child);
	}
}

void semantic_StmtList(struct Node *node){
	if(node == NULL || node->tokentype != _StmtList){
		printf("the node is not a StmtList\n");
		if(node != NULL)
			printf("node is %s\n", node->name);
		return;
	}
	struct Node *child = node->child;
	if(child != NULL){
		semantic_Stmt(child);
		child = child->brother;
		if(child != NULL)
			semantic_StmtList(child);
	}
}

void semantic_Stmt(struct Node *node){
	if(node == NULL || node->tokentype != _Stmt){
		printf("the node is not a Stmt\n");
		return;
	}
	struct Node *child = node->child;
	if(child->tokentype == _Exp){
		int left;
		enum VarType exptype;
		semantic_Exp(&left, child, &exptype);
	}
	else if(child->tokentype == _CompSt){
		symstack[++stackindex] = NULL;
		semantic_CompSt(child);
		symnode_t *temp = symstack[stackindex];
		while(temp != NULL){
			temp->layer = -1;
			temp = temp ->layernext;
		}
		stackindex --;
	}
	else if(child->tokentype == _RETURN){
		int left;
		enum VarType retype;
		union Varp vp = semantic_Exp(&left, child->brother, &retype);
		if(checkAssignType(nowFunc->retype, nowFunc->repointer, retype, vp) < 0)
			printf("Error type 8 at Line %d:return value is not the function expected\n", child->lineno);

	}else if(child->tokentype == _IF){
		child = child->brother->brother;
		int left;
		enum VarType vartp;
		semantic_Exp(&left, child, &vartp);
		child = child->brother->brother;
		semantic_Stmt(child);
		child = child->brother;
		if(child != NULL && child->tokentype == _ELSE){
			child = child->brother;
			semantic_Stmt(child);
		}
	}else if(child->tokentype == _WHILE){
		int left;
		enum VarType whiletype;
		child = child->brother->brother;
		semantic_Exp(&left, child, &whiletype);
		child = child->brother->brother;
		semantic_Stmt(child);

	}

}
void semantic_Def(struct Node *node){
	if(node == NULL || node->tokentype != _Def){
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
		if(structlayer > 0){
			printf("Error type 15 at Line %d:initialze filed when Structure define\n", child->lineno);
			return;
		}
		child = child->brother;
		int c ;
		vp = semantic_Exp(&c, child, &exptype);	
		if(exptype != V_ERROR && basetype != V_ERROR && checkAssignType(left->type, left->tp, exptype, vp) < 0){
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
			if(etp != V_ERROR && rightetp != V_ERROR && checkAssignType(etp, et, rightetp, rightrt) < 0){
				printf("Error type 5 at Line %d:left and right type can't match\n", child->lineno);
			}
			*leftorright = -1;//assign exp dont have left value
			*type = etp;
			return et;
		}else if(child->tokentype == _LB){//array 
			if(etp != V_ARRAY){
				printf("Error type 10 at Line %d:the exp is not an array\n", child->lineno);
				*type = V_ERROR;
				*leftorright = -1;
				union Varp r;
				r.sp = NULL;
				return r;
			}
			child = child->brother;
			enum VarType indextype;
			semantic_Exp(&left, child, &indextype);
			if(indextype != V_INT){
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
			enum VarType dottype;
			child = child->brother;
			if(etp != V_STRUCT){
				printf("Error type 13 at Line %d:use Dot with a var isn't a structure\n", child->lineno);
				*type = V_ERROR;
				*leftorright = -1;
				union Varp v ;
				v.sp = NULL;
				return v;
			}
			union Varp dotvp = getStructureMem(et.sp, child, &dottype);
			if(dottype == V_ERROR)
				printf("Error type 14 at Line %d:the struct don't have the menber wanted\n", child->lineno);
			*type = dottype;
			return dotvp;
			*leftorright = 1;//structer member has left val;
		}else{//op
			child = child->brother;
			enum VarType sectype;
			semantic_Exp(&left, child, &sectype);
			if(sectype != V_ERROR && etp != V_ERROR && (sectype != etp || sectype == V_STRUCT || sectype == V_ARRAY\
					 || etp == V_STRUCT || etp == V_ARRAY)){// V_ERROR 布报本错误,因为这是其他错误引起的
				printf("Error type 7 at Line %d:oprator cant match\n", child->lineno);
			}
			*leftorright = -1;//op exp didn't have left value
			*type = etp;
			return et;
		}
	}else if(child->tokentype == _LP){
		child = child->brother;
		return semantic_Exp(leftorright, child, type);
	}else if(child->tokentype == _ID){	
	
		if(child->brother == NULL){
			*leftorright = 1;
			union Varp revp =  getVar(child, type);
			if(*type == V_ERROR)
				printf("Error type 1 at Line %d: use old var didn't dec\n", child->lineno);
			return revp;
		}
		child = child->brother;
		struct FuncMsg *fc = getFuncMsg(node->child);
		if(fc == NULL){
			enum VarType maybevartype;
			getVar(node->child, &maybevartype);
			if(maybevartype == V_ERROR)
				printf("Error type 2 at Line %d:call a function didn't exist\n", child->lineno);
			else
				printf("Error type 11 at Line %d:call a varibale as a function\n", child->lineno);

		}else{
			struct ParList * arg = NULL;		
			if(child->brother->tokentype == _Args){
				arg = semantic_Args(child->brother);
			//	printf("is arg is NULL ? %d\n", arg == NULL);
			}
			if(checkArg(fc, arg) < 0)
				printf("Error type 9 at Line %d:call a function the argscant match\n", child->lineno);
		}
	}else if(child->tokentype == _MINUS){
		child = child->brother;
		int right;
		*leftorright = -1;//this will not hava left value;
		union Varp re = semantic_Exp(&right, child, type);
		if(*type == V_STRUCT || *type != V_ARRAY || *type == V_ERROR)
			printf("Error type 7 at Line %d:oprator cant match\n", child->lineno);
		return re;
	}else if(child->tokentype == _NOT){
		child = child->brother;
		int right;
		*leftorright = -1;
		union Varp re = semantic_Exp(&right, child, type);
		if(*type != V_INT)
			printf("Error type 7 at Line %d:oprator cant match\n", child->lineno);
		return re;
	}else if(child->tokentype == _INT){
		*leftorright = -1;
		*type = V_INT;
		union Varp re;
		re.sp = NULL;
		return re;
	}else if(child->tokentype == _FLOAT){
		*leftorright = -1;
		*type = V_FLOAT;
		union Varp re;
		re.sp = NULL;
		return	re;
	}
	union Varp r;
	r.sp = NULL;
	return r;
}

struct ParList * semantic_Args(struct Node *node){
	if(node == NULL || node->tokentype != _Args){
		printf("the node is not a Args\n");
		return NULL;
	}
	struct Node *child = node->child;
	int left;
	enum VarType exptype;
	union Varp vp = semantic_Exp(&left, child, &exptype);
	struct ParList *the = malloc(sizeof(struct ParList));
	the->partype = exptype;
	the->parpointer = vp;
//	printf("vp is int ? %d ", exptype == V_INT);
//	printf("is error ? %d\n", exptype == V_ERROR);
	the->name = node->name;
	the->next = NULL;
	child = child->brother;
	if(child != NULL){
		child = child->brother;
		the->next =	semantic_Args(child);
	}
	return the;
}

unsigned int makehash(char * name){
	//use SDB hash
	unsigned int hash = 0;
	while (*name){ // equivalent to: hash = 65599*hash + (*str++);
		hash = (*name++) + (hash << 6) + (hash << 16) - hash;	             
	}
	return (hash & 0x7FFFFFFF) % HASHLEN;
}

struct VariMsg *newVar(struct Node *node, enum VarType basetype, union Varp vp){
	if(node == NULL || node->tokentype != _ID){
		printf("the node is not a ID\n");
		return NULL;
	}
	unsigned int hash = makehash(node->name);
	symnode_t * temp = symtable[hash];
	while(temp != NULL){
		if(temp->smb.symtype == S_VARI){
			if(temp->layer == stackindex && strcmp(temp->smb.msgp.vmsgp->name, node->name) == 0){
				//报错.相同作用域下有同名变量
				if(structlayer > 0)
					printf("Error type 15 at Line %d:redefind field, old define at line %d\n", node->lineno, temp->smb.lineNumber);
				else
					printf("Error type 3 at Line %d:conflict type, old define at line %d \n", node->lineno, temp->smb.lineNumber);
				return NULL;
			}
		}
		temp = temp->next;
	}
	//if not return then can crate a new node
	symnode_t * newvarnode =(symnode_t*) malloc(sizeof(symnode_t));
	newvarnode->layer = stackindex;
	if(funarg == 1 && (basetype == V_ARRAY || basetype == V_STRUCT))
		newvarnode->flag = 1;
	else
		newvarnode->flag = 0;
	newvarnode->layernext = NULL;
	newvarnode->next = NULL;
	if(symtable[hash] != NULL)
		newvarnode->next = symtable[hash]->next;
	symtable[hash] = newvarnode;
	if(symstack[stackindex] == NULL){
		symstack[stackindex] = newvarnode;
		currenttail = newvarnode;
	}else
		currenttail->layernext = newvarnode;
	newvarnode->smb.symtype = S_VARI;
	newvarnode->smb.visitedTag = 1;
	newvarnode->smb.lineNumber = node->lineno;
	newvarnode->smb.msgp.vmsgp = malloc(sizeof(struct VariMsg));
	newvarnode->smb.msgp.vmsgp->name = node->name;
	newvarnode->smb.msgp.vmsgp->var_no = vaCount ++;
	newvarnode->smb.msgp.vmsgp->type = basetype;
	newvarnode->smb.msgp.vmsgp->tp = vp;
	return newvarnode->smb.msgp.vmsgp;
}


struct VariMsg *newArrayVar(struct Node *node, union Varp arraybase){
	if(node == NULL || node->tokentype != _VarDec){
		printf("the node is not a VarDec, @newArrayVar\n");
		return NULL;
	}
	struct Node *child = node->child;
	if(child->tokentype == _ID){
		struct VariMsg * remsg = (struct VariMsg*)malloc(sizeof(struct VariMsg));
		remsg->type = V_ARRAY;
		remsg->name = child->name;
		remsg->var_no = vaCount++;
	//	printf("array name is %s;\n", child->name);
		remsg->tp = arraybase;
		unsigned int hash = makehash(child->name);
		symnode_t * temp = symtable[hash];
		while(temp != NULL){
			if(temp->smb.symtype == S_VARI){
				if(temp->layer == stackindex && strcmp(temp->smb.msgp.vmsgp->name, child->name) == 0){
					//报错.相同作用域下有同名变量
					printf("Error type 3 at Line %d:conflict type, old define at line %d \n", child->lineno, temp->smb.lineNumber);
					free(remsg);//清理
					return NULL;
				}
			}
			temp = temp->next;
		}
		//if not return then can crate a new node
		symnode_t * newvarnode = malloc(sizeof(symnode_t));
		newvarnode->layer = stackindex;
		newvarnode->layernext = NULL;
		newvarnode->next = NULL;
		if(symtable[hash] != NULL)
			newvarnode->next = symtable[hash]->next;
		symtable[hash] = newvarnode;
		if(symstack[stackindex] == NULL){
			symstack[stackindex] = newvarnode;
			currenttail = newvarnode;
		}else
			currenttail->layernext = newvarnode;
		newvarnode->smb.symtype = S_VARI;
		newvarnode->smb.visitedTag = 1;
		newvarnode->smb.lineNumber = node->lineno;
		newvarnode->smb.msgp.vmsgp = remsg;
	//	printf("create a new array, name %s, hash %d\n",child->name, hash);
		return newvarnode->smb.msgp.vmsgp;
	}
	else if(child->tokentype == _VarDec){	
		union Varp arraybase;
		arraybase.ap = malloc(sizeof(struct ArrayMsg));	
		arraybase.ap->basetype = V_ARRAY;
		arraybase.ap->base = arraybase;
		arraybase.ap->size = getIntVal(child->brother->brother);
		return newArrayVar(child, arraybase);//递归的搞定
	}
	return NULL;
}

int getIntVal(struct Node *node){
	if(node == NULL || node->tokentype != _INT){
		printf("the node is not a INT, @getIntVal\n");
		return 0;
	}
	return atoi(node->name);
}

struct FuncMsg *newFunction(struct Node *node, enum VarType retype, struct Structure *restrp, struct ParList *args, int dord){
	if(node == NULL || node->tokentype != _ID){
		printf("the node is not a ID, @newFunction\n");
		return NULL;
	}
	unsigned int hash = makehash(node->name);
	funnode_t * temp = funtable[hash];
	while(temp != NULL){
		if(strcmp(temp->fun->name, node->name) == 0){
			union Varp restrpvp;
			restrpvp.sp = restrp;
			if(dord < 0 || temp->fun->dord < 0){//两次至少一次是声明
				if(checkAssignType(temp->fun->retype, temp->fun->repointer, retype, restrpvp) < 0 || checkArg(temp->fun, args) < 0){
					printf("Error type 19 at Line %d:conflict function\n", node->lineno);
					nowerror = 1;
					break;
				}
				// 如果一致则返回
				// 并且如果本次是定义,则修改函数信息为定义过
				if(dord > 0)
					temp->fun->dord = dord;
				return temp->fun;
			}
			printf("Error type 4 at Line %d:has old define\n", node->lineno);
			nowerror = 1;
			break;//means redefine
		}
		temp = temp->next;
	}
	//means can new a new funtion
	funnode_t * newfun = malloc(sizeof(funnode_t));
	newfun->next = NULL;
	newfun->fun = malloc(sizeof(struct FuncMsg));
	newfun->fun->arglist = args;
	newfun->fun->retype = retype;
	newfun->fun->name = node->name;
	newfun->fun->repointer.sp = restrp;
	newfun->fun->dord = dord;
	if(nowerror == 0){//没有出错时才将新函数添加到函数表
		if(funtable[hash] != NULL)
			newfun->next = funtable[hash]->next;
		funtable[hash] = newfun;
		return newfun->fun;
	}else{
		struct FuncMsg *tore = newfun->fun;
		free(newfun);
		return tore;
	}
	return NULL;
}

struct Structure *newStruct(struct Node *idnode, struct Node *node){
	if(idnode == NULL){
		stackindex ++;
		structlayer ++;
		symstack[stackindex] = NULL;
		struct Structure * re = malloc(sizeof(struct Structure));
		semantic_DefList(node);
		re->name = NULL;//匿名结构体, 匿名结构体不存在同名关系	
		re->memlist = symstack[stackindex];
		symnode_t *tofree = symstack[stackindex];
		while(tofree != NULL){
			tofree->layer = -1;
			tofree = tofree->layernext;
		}
		stackindex --;
		structlayer --;
		return re;
	}
	if(idnode->tokentype != _ID){
		printf("the node is not a ID, @newStruct\n");
		return NULL;
	}
	if(node == NULL || node->tokentype != _DefList){
		printf("the node is not a DefList but a %s, @newStruct\n", node->name);
		return NULL;
	}
	//先在struct表中查询struct 找到是否有struct同名
	//然后解析其中的目
	symtable[++stackindex] = NULL ;//一层作用域
	unsigned int hash = makehash(idnode->name);
	strnode_t * temp = strtable[hash];
	int flag = 0;
//	printf("newStruct\n");
	while(temp != NULL){
		if(strcmp(temp->str.name, idnode->name) == 0){
			printf("Error type 16 at Line %d: has old define\n", idnode->lineno);
			flag = 1;
			break;
		}	
		temp = temp->next;
	}
	//printf("out of loop\n");
	structlayer ++;
	strnode_t *newstr = malloc(sizeof(strnode_t));
	newstr->next = NULL;
	newstr->str.name = idnode->name;
	semantic_DefList(node);
	newstr->str.memlist = symstack[stackindex];//当前作用域下生成的所有变量都是他的
	symnode_t * tofree = symstack[stackindex];
	structlayer --;
	while(tofree != NULL){
		tofree->layer = -1;//means free
		tofree = tofree->layernext;
	//	printf("tofree\n");
	}
	if(flag){
		free(newstr);
		newstr = NULL;
		return NULL;
	}
	if(strtable[hash] != NULL){
		newstr->next = strtable[hash]->next;
	}
	strtable[hash] = newstr;
	return &(newstr->str);
}

struct Structure *getStructureByName(char *name){
	if(name == NULL || strlen(name) == 0){
		printf("the name is NULL\n");
		return NULL;
	}
	unsigned int hash = makehash(name);
	strnode_t * strtemp = strtable[hash];
//	printf("get Structure by name\n");
	while(strtemp != NULL){
		if(strcmp(strtemp->str.name, name) == 0){
			return &(strtemp->str);
		}
		strtemp = strtemp->next;	
	}
	return NULL;
}


union Varp getStructureMem(struct Structure *sp, struct Node *memnode, enum VarType *memtype){
	if(sp == NULL || memnode == NULL || memnode->tokentype != _ID){
		printf("use of getStructureMem error\n");
		*memtype = V_ERROR;
		union Varp r;
		r.sp = NULL;
		return r;
	}
	symnode_t * temp = sp->memlist;
	while(temp != NULL){
		if(strcmp(temp->smb.msgp.vmsgp->name, memnode->name) == 0){
			if(temp->smb.symtype == S_VARI){
				*memtype = temp->smb.msgp.vmsgp->type;
				return temp->smb.msgp.vmsgp->tp;
			}
		}
		temp = temp->layernext;
	}
	//cant find ,
	*memtype = V_ERROR;
	union Varp r;
	r.sp = NULL;
	return r;
}

union Varp getVar(struct Node *idnode, enum VarType *vtp){
	if(idnode == NULL || idnode->tokentype != _ID){
		printf("the node is not a ID\n");
		*vtp = V_ERROR;
		union Varp r;
		r.sp =NULL;
		return r;
	}
	unsigned int hash = makehash(idnode->name);
//	printf("want name is %s, hash %d\n", idnode->name, hash);
	symnode_t * temp = symtable[hash];
	int maxlay = 0;
	symnode_t * find = NULL;
	while(temp != NULL){
	//	printf("name is %s, want %s\n", temp->smb.msgp.vmsgp->name, idnode->name);
		if(temp->smb.symtype == S_VARI && strcmp(temp->smb.msgp.vmsgp->name, idnode->name) == 0){	
			if(1 || (temp->layer >= 0 && temp->layer >= maxlay)){
				find = temp;
				maxlay = temp->layer;
			}
		}
		temp = temp->next;
	}
	union Varp r;
	if(find == NULL){
		*vtp = V_ERROR;
		r.sp = NULL;
	}
	else{
		*vtp = find->smb.msgp.vmsgp->type;
		r = find->smb.msgp.vmsgp->tp;
	//	printf("find Var name is %s\n", r);
	}
	return r;
}
int getVarID(struct Node *idnode){
	if(idnode == NULL || idnode->tokentype != _ID){
		printf("the node is not a ID\n");		
		return -1;
	}
	unsigned int hash = makehash(idnode->name);
//	printf("want name is %s, hash %d\n", idnode->name, hash);
	symnode_t * temp = symtable[hash];
	int maxlay = 0;	
	while(temp != NULL){
	//	printf("name is %s, want %s\n", temp->smb.msgp.vmsgp->name, idnode->name);
		if(temp->smb.symtype == S_VARI && strcmp(temp->smb.msgp.vmsgp->name, idnode->name) == 0){	
			return temp->smb.msgp.vmsgp->var_no;	
		}
		temp = temp->next;
	}
	return -1;

}
int getVarIDbyName(char*name){
	if(name == NULL || strlen(name) == 0){
		printf("getVarIDbyName, but name is empty\n");
		return -1;
	}

	unsigned int hash = makehash(name);
//	printf("want name is %s, hash %d\n", idnode->name, hash);
	symnode_t * temp = symtable[hash];
	int maxlay = 0;	
	while(temp != NULL){
	//	printf("name is %s, want %s\n", temp->smb.msgp.vmsgp->name, idnode->name);
		if(temp->smb.symtype == S_VARI && strcmp(temp->smb.msgp.vmsgp->name, name) == 0){	
			return temp->smb.msgp.vmsgp->var_no;
		}
		temp = temp->next;
	}
	return -1;
}
int getFlag(char*name){
	if(name == NULL || strlen(name) == 0){
		printf("getVarIDbyName, but name is empty\n");
		return -1;
	}

	unsigned int hash = makehash(name);
//	printf("want name is %s, hash %d\n", idnode->name, hash);
	symnode_t * temp = symtable[hash];
	int maxlay = 0;	
	while(temp != NULL){
	//	printf("name is %s, want %s\n", temp->smb.msgp.vmsgp->name, idnode->name);
		if(temp->smb.symtype == S_VARI && strcmp(temp->smb.msgp.vmsgp->name, name) == 0){	
			return temp->flag;
		}
		temp = temp->next;
	}
	return -3;
}
struct FuncMsg * getFuncMsg(struct Node *node){
	if(node == NULL || node->tokentype != _ID){
		printf("the node is not a ID, @getFuncMsg\n");
		return NULL;
	}
	unsigned int hash = makehash(node->name);
	funnode_t * temp = funtable[hash];
	while(temp != NULL){
		if(strcmp(temp->fun->name, node->name) == 0){
			return temp->fun;
		}
		temp = temp->next;
	}
	return NULL;
}

int checkAssignType(enum VarType lefttype, union Varp leftvp, enum VarType righttype, union Varp rightvp){

//	printf("checkArg : left %d, right %d\n", lefttype == V_INT, righttype == V_INT);
	if(lefttype != righttype || lefttype == V_ERROR || righttype == V_ERROR){
		return -1;
	}
	if(lefttype == V_ARRAY){
		return checkAssignType(leftvp.ap->basetype, leftvp.ap->base, rightvp.ap->basetype, rightvp.ap->base);
	}
	if(lefttype == V_STRUCT){
		symnode_t *temp1 = leftvp.sp->memlist;
		symnode_t *temp2 = rightvp.sp->memlist;
	/*	symnode_t *t1 = temp1;
		symnode_t *t2 = temp2;
		while(t1 != NULL){
			printf("t1 .member :%s\n", t1->smb.msgp.vmsgp->name);
			t1 = t1->layernext;
		}
		while(t2 != NULL){
			printf("t2 .member :%s\n", t2->smb.msgp.vmsgp->name);
			t2 = t2->layernext;
		}*/
		while(temp1 != NULL && temp2 != NULL){
			int rc = checkAssignType(temp1->smb.msgp.vmsgp->type, temp1->smb.msgp.vmsgp->tp, temp2->smb.msgp.vmsgp->type, temp2->smb.msgp.vmsgp->tp);
			if(rc < 0){
		//		printf("t1 name %s ,t2 name %s\n", temp1->smb.msgp.vmsgp->name, temp2->smb.msgp.vmsgp->name);
		//		printf("t1.x != t2.x\n");
				return -1;
			}
	//			printf("t1 name %s ,t2 name %s\n", temp1->smb.msgp.vmsgp->name, temp2->smb.msgp.vmsgp->name);
	//		printf("t1.x ==  t2.x\n");
			temp1 = temp1->layernext;
			temp2 = temp2->layernext;
		}
		if(temp1 != NULL || temp2 != NULL){
	//		printf("t1.len != t2.len\n");
	//		printf("t1layer %d, name %s\n", temp1->layer, temp1->smb.msgp.vmsgp->name);
	//		printf("t1.p %ld, t2.d %ld\n", temp1, temp2);
			return -1;
		}
	}
//	printf("t1 == t2\n");
	return 1;
}

int checkArg(struct FuncMsg *fc, struct ParList *arg){
	if(fc == NULL)
		return -1;
	struct ParList *fcargs = fc->arglist;
	while(fcargs != NULL && arg != NULL){
		int rc = checkAssignType(fcargs->partype, fcargs->parpointer, arg->partype, arg->parpointer);
		if(rc < 0)
			return -1;
		fcargs = fcargs->next;
		arg = arg->next;
	}
//	printf("fcargs %d, arg %d\n", fcargs, arg);
	if(fcargs != NULL || arg != NULL)
		return -1;
	return 1;
}
