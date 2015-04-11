#ifndef LEXICAL_H
#define LEXICAL_H
#include<stdarg.h>
enum Type{END, NOTEND};
enum TokenType{
	_INT, _FLOAT, _ID, _SEMI, _COMMA, _ASSIGNOP, _RELOP, _PLUS, _MINUS, _STAR, _DIV, _AND, _OR, _DOT, _NOT, _TYPE, _LP, _RP, _LB, _RB, _LC, _RC, _STRUCT, _RETURN, _IF, _ELSE, _WHILE, _Program, _ExtDefList, _ExtDef, _ExtDecList, _Specifier, _StructSpecifier, _OptTag, _Tag, _VarDec, _FunDec, _VarList, _ParamDec, _CompSt, _StmtList, _Stmt, _DefList, _Def, _DecList, _Dec, _Exp, _Args
};
struct Node{
	int lineno;
	enum Type type;
	enum TokenType tokentype;
	char *name;
	struct Node *brother;//左兄弟,右孩子结构的树
	struct Node *child;
};

struct Node *createNode(int row, enum Type tp, enum TokenType tt, char *nm);
struct Node *destroyNode(struct Node *gb);
struct Node *addChild(struct Node *ft, int cn, ...);
struct Node *printTree(struct Node *rt, int nLayer);
extern struct Node *root;
extern int yylineno;
extern int error;
#endif
