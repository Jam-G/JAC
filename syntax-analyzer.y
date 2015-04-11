%{
#include<stdio.h>
#include"lexical.h"
#include"lex.yy.c"
%}
%union{
	struct Node * type_node;
	int type_int;
	float type_float;
}
/*declared types*/

/*declared tokens*/
%token  <type_node> INT FLOAT ID TYPE SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR 
%token  <type_node> DIV AND OR DOT NOT LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%right ASSIGNOP NOT
%left OR AND RELOP PLUS MINUS STAR DIV DOT LB RB LP RP
%nonassoc ELSE STRUCT RETURN WHILE

/*declared non-terminals*/
%type <type_node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier 
%type <type_node> OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
%%
Program:ExtDefList{
	$$=createNode(@$.first_line, NOTEND, _Program, "Program");
	addChild($$, 1, $1);	
	root = $$;   
};
ExtDefList:ExtDef ExtDefList{ 
	$$=createNode(@$.first_line, NOTEND, _ExtDefList, "ExtDefList");
	addChild($$, 2, $1, $2);
}
		  |{$$=NULL;};
ExtDef:Specifier ExtDecList SEMI{
	$$=createNode(@$.first_line, NOTEND, _ExtDef, "ExtDef");
	addChild($$, 3, $1, $2, $3);
}
	  |Specifier SEMI{
	$$=createNode(@$.first_line, NOTEND, _ExtDef, "ExtDef");
	addChild($$, 2, $1, $2);
}
	  |Specifier FunDec CompSt{
	$$=createNode(@$.first_line, NOTEND, _ExtDef, "ExtDef");
	addChild($$, 3, $1, $2, $3);
}
	| error SEMI{haserror = 1;printf("Define Missing ';' before column:%d\n", @$.first_column);};
ExtDecList:VarDec{
	$$=createNode(@$.first_line, NOTEND, _ExtDecList, "ExtDecList");
	addChild($$, 1, $1);
}
		  |VarDec COMMA ExtDecList{
	$$=createNode(@$.first_line, NOTEND, _ExtDecList, "ExtDecList");
	addChild($$, 3, $1, $2, $3);
};
Specifier:TYPE{
	$$=createNode(@$.first_line, NOTEND, _Specifier, "Specifier");
	addChild($$, 1, $1);
}
		 |StructSpecifier{
	$$=createNode(@$.first_line, NOTEND, _Specifier, "Specifier");
	addChild($$, 1, $1);
};
StructSpecifier:STRUCT OptTag LC DefList RC{
	$$=createNode(@$.first_line, NOTEND, _StructSpecifier, "StructSpecifier");
	addChild($$, 5, $1, $2, $3, $4, $5);
}
			   |STRUCT Tag{
	$$=createNode(@$.first_line, NOTEND, _StructSpecifier, "StructSpecifier");
	addChild($$, 2, $1, $2);
};
OptTag:ID{
	$$=createNode(@$.first_line, NOTEND, _OptTag, "OptTag");
	addChild($$, 1, $1);	
}
	  |{$$=NULL;};
Tag:ID{
	$$=createNode(@$.first_line, NOTEND, _Tag, "Tag");
	addChild($$, 1, $1);
};
VarDec:ID{
	$$=createNode(@$.first_line, NOTEND, _VarDec, "VarDec");
	addChild($$, 1, $1);
}
	  |VarDec LB INT RB{
	$$=createNode(@$.first_line, NOTEND, _VarDec, "VarDec");
	addChild($$, 4, $1, $2, $3, $4);
};
FunDec:ID LP VarList RP{
	$$=createNode(@$.first_line, NOTEND, _FunDec, "FunDec");
	addChild($$, 4, $1, $2, $3, $4);
}
	  |ID LP RP{
	$$=createNode(@$.first_line, NOTEND, _FunDec, "FunDec");
	addChild($$, 3, $1, $2, $3);
}
	|error RP{haserror = 1; printf("Function Missing ')' before column:%d\n", @$.first_column);};
VarList:ParamDec COMMA VarList{
	$$=createNode(@$.first_line, NOTEND, _VarList, "VarList");
	addChild($$, 3, $1, $2, $3);
}
	   |ParamDec{
	$$=createNode(@$.first_line, NOTEND, _VarList, "VarList");
	addChild($$, 1, $1);
};
ParamDec:Specifier VarDec{
	$$=createNode(@$.first_line, NOTEND, _ParamDec, "ParamDec");
	addChild($$, 2, $1, $2);
};
CompSt:LC DefList StmtList RC{
	$$=createNode(@$.first_line, NOTEND, _CompSt, "CompSt");
	addChild($$, 4, $1, $2, $3, $4);
};
StmtList:Stmt StmtList{
	$$=createNode(@$.first_line, NOTEND, _StmtList, "StmtList");
	addChild($$, 2, $1, $2);
}
		|{$$=NULL;};
Stmt:Exp SEMI{
	$$=createNode(@$.first_line, NOTEND, _Stmt, "Stmt");
	addChild($$, 2, $1, $2);
}
	|CompSt{
	$$=createNode(@$.first_line, NOTEND, _Stmt, "Stmt");
	addChild($$, 1, $1);
}
	|RETURN Exp SEMI{
	$$=createNode(@$.first_line, NOTEND, _Stmt, "Stmt");
	addChild($$, 3, $1, $2, $3);
}
	|IF LP Exp RP Stmt{
	$$=createNode(@$.first_line, NOTEND, _Stmt, "Stmt");
	addChild($$, 5, $1, $2, $3, $4, $5);
}
	|IF LP Exp RP Stmt ELSE Stmt{
	$$=createNode(@$.first_line, NOTEND, _Stmt, "Stmt");
	addChild($$, 7, $1, $2, $3, $4, $5, $6, $7);
}
	|WHILE LP Exp RP Stmt{
	$$=createNode(@$.first_line, NOTEND, _Stmt, "Stmt");
	addChild($$, 5, $1, $2, $3, $4, $5);
}
	|error RP{haserror = 1; printf("Statment Missing ')' before column:%d\n", @$.last_column);}
	|error SEMI{haserror = 1; printf("Statment Missing ';' before column:%d\n", @$.last_column);};
DefList:Def DefList{
	$$=createNode(@$.first_line, NOTEND, _DefList, "DefList");
	addChild($$, 2, $1, $2);
}
	   |{$$=NULL;};
Def:Specifier DecList SEMI{
	$$=createNode(@$.first_line, NOTEND, _Def, "Def");
	addChild($$, 3, $1, $2, $3);
};
DecList:Dec{
	$$=createNode(@$.first_line, NOTEND, _DecList, "DecList");
	addChild($$, 1, $1);
}
	   |Dec COMMA DecList{
	$$=createNode(@$.first_line, NOTEND, _DecList, "DecList");
	addChild($$, 3, $1, $2, $3);
};
Dec:VarDec{
	$$=createNode(@$.first_line, NOTEND, _Dec, "Dec");
	addChild($$, 1, $1);
}
   |VarDec ASSIGNOP Exp{
	$$=createNode(@$.first_line, NOTEND, _Dec, "Dec");
	addChild($$, 3, $1, $2, $3);
};
Exp:Exp ASSIGNOP Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp AND Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp OR Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp RELOP Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp PLUS Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp MINUS Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp STAR Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp DIV Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |LP Exp RP{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |MINUS Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 2, $1, $2);
}
   |NOT Exp{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 2, $1, $2);
}
   |ID LP Args RP{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 4, $1, $2, $3, $4);
}
   |ID LP RP{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |Exp LB Exp RB{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 4, $1, $2, $3, $4);
}
   |Exp DOT ID{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 3, $1, $2, $3);
}
   |ID{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 1, $1);
}
   |INT{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 1, $1);
}
   |FLOAT{
	$$=createNode(@$.first_line, NOTEND, _Exp, "Exp");
	addChild($$, 1, $1);
};

Args:Exp COMMA Args{
	$$=createNode(@$.first_line, NOTEND, _Args, "Args");
	addChild($$, 3, $1, $2, $3);
}
	|Exp{
	$$=createNode(@$.first_line, NOTEND, _Args, "Args");
	addChild($$, 1, $1);
}

%%

int yyerror(){
	printf("Error type B at line %d: ", yylineno);
}
