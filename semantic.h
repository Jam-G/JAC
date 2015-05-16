#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "lexical.h"
#include<assert.h>
enum SymbolType{
	S_VARI, S_FUNC
};
enum VarType{
	V_INT, V_FLOAT, V_STRUCT, V_ARRAY, V_ERROR
};
union Varp{
	struct Structure *sp;
	struct ArrayMsg *ap;
};
struct ParList{
	enum VarType partype;
	union Varp parpointer;//if is a struct or arrayy ,else it will be NULL
	char *name;
	struct ParList *next;
};
struct Structure{
	char *name;
	struct symbolHashNode *memlist;
};
struct ArrayMsg{
	int size;//array size
	enum VarType basetype;//if basis type is array ,means k>1 dimensions array
	union Varp base;//if base is not int or float ,this will poin to the struct or arry
	union Varp nextd;//next dimension
};
struct FuncMsg{
	char *name;
	enum VarType retype;
	int dord;
	union Varp repointer;//if retype is not int or float then this pointer point to the type(maybe an array or a structure)
	struct ParList *arglist;
};
struct VariMsg{
	char *name;
	enum VarType type;//int or float or struct or array
	union Varp tp;//type pointer, point to the type of this variable if it not the basis type (int or float)
};
union SymMsgp{
	struct FuncMsg *fmsgp;
	struct VariMsg *vmsgp;
};
struct Symbol{
	enum SymbolType symtype;
	int visitedTag;
	int lineNumber;
	union SymMsgp msgp;//var or function
};

typedef struct symbolHashNode{
	struct Symbol smb;
	int layer;
	struct symbolHashNode *next;
	struct symbolHashNode *layernext;
}symnode_t;
typedef struct structureHashNode{
	struct Structure str;
	struct structureHashNode *next;
}strnode_t;
typedef struct functionHashNode{
	struct FuncMsg *fun;//point to the symbol in symboltable
	struct functionHashNode *next;
}funnode_t;
extern void inittables();
extern void semantic();
extern void checkNoDeFun();
extern void semantic_program(struct Node *node);
extern void semantic_ExtDefList(struct Node *node);
extern void semantic_ExtDef(struct Node *node);
extern struct Structure* semantic_Specifier(struct Node *node, enum VarType *basetype);
extern void semantic_ExtDecList(struct Node *node, enum VarType basetype, struct Structure *strp);
extern void semantic_FunDec(struct Node *node, enum VarType, struct Structure *strp, int dord);
extern void semantic_CompSt(struct Node *node);
extern struct Structure *semantic_StructSpecifier(struct Node *node, enum VarType *basetype);
extern struct VariMsg * semantic_VarDec(struct Node *node, enum VarType basetype, struct Structure *strp);
extern struct ParList* semantic_VarList(struct Node *node);
extern void semantic_ParamDec(struct Node *node, enum VarType *type, union Varp *varp, char **name);
extern void semantic_DefList(struct Node *node);
extern void semantic_StmtList(struct Node *node);
extern void semantic_Stmt(struct Node *node);
extern void semantic_Def(struct Node *node);
extern void semantic_DecList(struct Node *node, enum VarType type, struct Structure *strp);
extern void semantic_Dec(struct Node *node, enum VarType basetype, struct Structure *strp);
extern union Varp semantic_Exp(int *leftorright, struct Node *node, enum VarType *type);
extern struct ParList *semantic_Args(struct Node *node);
//check the struct id exist if in the table can't find
//the struct means has not defined then return -1, else return 1
//and this function need check the condation that the structure don't have name
//in which will retrun 1
extern struct Structure * getStructureByName(char *name);
extern struct Structure * newStruct(struct Node *idnode, struct Node *node);
extern struct VariMsg * newVar(struct Node *node, enum VarType basetype, union Varp vtp);
extern struct VariMsg * newArrayVar(struct Node *node, union Varp arraybase);
extern struct FuncMsg * newFunction(struct Node *idnode, enum VarType retype, struct Structure *restrp, struct ParList *args, int dord); 
extern int getIntVal(struct Node *intnode);
extern int checkAssignType(enum VarType lefttype, union Varp leftp, enum VarType righttype, union Varp rightp);
extern union Varp getVar(struct Node *idnode, enum VarType *vtp);
extern union Varp getStructureMem(struct Structure * sp, struct Node *memnode, enum VarType *memtype);
extern struct FuncMsg *getFuncMsg(struct Node *node);
extern int checkArg(struct FuncMsg *fc, struct ParList *arg);
extern unsigned int makehash(char *name);
#endif
