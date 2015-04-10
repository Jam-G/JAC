#ifndef LEXICAL_H
#define LEXICAL_H
#include<stdarg.h>
enum Type{END, NOTEND};
enum TokenType{
	INT, FLOAT, ID, SEMI, COMMA, ASSIGNOP, RELOP, PLUS, MINUS, STAR, DIV, AND, OR, DOT, NOT, TYPE, LP, RP, LB, RB, LC, RC, STRUCT, RETURN, IF, ELSE, WHILE
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

#endif
