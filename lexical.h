#ifndef LEXICAL_H
#define LEXICAL_H
#include<stdarg.h>
struct Node{
	int lineno;
	int type;
	int tokentype;
	char *name;
	struct Node *brother;//左兄弟,右孩子结构的树
	struct Node *child;
};

struct Node *createNode(int row, int tp, int tt, char *nm);
struct Node *destroyNode(struct Node *gb);
struct Node *addChild(struct Node *ft, int cn, ...);
struct Node *printTree(struct Node *rt, int nLayer);

#endif
