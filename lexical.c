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
		addpoint = addpoint ->brother;
		if(addpoint == NULL)
			break;
	}
	return addpoint;
}

struct Node *VLRprintTree(struct Node *rt){
	if(rt == NULL)
		return NULL;
	return NULL;
}
