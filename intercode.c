#include "lexical.h"
#include "semantic.h"
#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "sys/ioctl.h"
#include "stdlib.h"
#include "stdarg.h"
#include "intercode.h"

int temp_count = 0;
int label_count = 0;
//FILE *fp;

//output
void output_operand(Operand op) {
	if(op == NULL) return;
	switch(op->kind){
	case OP_VARIABLE:
		fprintf(fp, "v%d", op->u.var_no);
		break;
	case OP_TEMP:
		fprintf(fp, "t%d", op->u.temp_no);
		break;
	case OP_CONSTANT:
		fprintf(fp, "#%d", op->u.value);
		break;
	case OP_REFERENCE:
		fprintf(fp, "&v%d", op->u.var_no);
		break;
	case OP_ADDRESS:	
		fprintf(fp, "*t%d", op->u.temp_no);
		break;
	case OP_LABEL:
		fprintf(fp, "label%d", op->u.label_no);
		break;
	case OP_FUNCTION:
		fprintf(fp, "%s", op->u.func_name);
		break;
	case OP_PARAM:
		fprintf(fp, "%s", op->u.param_name);
		break;
	}
}

void output_reltype(RelType type) {
	switch(type){
	case EQ:
		fputs(" == ", fp);
		break;
	case NE:
		fputs(" != ", fp);
		break;
	case MT:
		fputs(" > ", fp);
		break;
	case LT:
		fputs(" < ", fp);
		break;
	case ME:
		fputs(" >= ", fp);
		break;
	case LE:
		fputs(" <= ", fp);
		break;
	}
}

void output_assign(InterCode code) {
	output_operand(code->u.assign.left);
	fputs(" := ", fp);
	output_operand(code->u.assign.right);
	fputs("\n", fp);
}

void output_add(InterCode code) {
	output_operand(code->u.binop.result);
	fputs(" := ", fp);
	output_operand(code->u.binop.op1);
	fputs(" + ", fp);
	output_operand(code->u.binop.op2);
	fputs("\n", fp);
}

void output_sub(InterCode code) {
	output_operand(code->u.binop.result);
	fputs(" := ", fp);
	output_operand(code->u.binop.op1);
	fputs(" - ", fp);
	output_operand(code->u.binop.op2);
	fputs("\n", fp);
}

void output_mul(InterCode code) {
	output_operand(code->u.binop.result);
	fputs(" := ", fp);
	output_operand(code->u.binop.op1);
	fputs(" * ", fp);
	output_operand(code->u.binop.op2);
	fputs("\n", fp);
}

void output_div(InterCode code) {
	output_operand(code->u.binop.result);
	fputs(" := ", fp);
	output_operand(code->u.binop.op1);
	fputs(" / ", fp);
	output_operand(code->u.binop.op2);
	fputs("\n", fp);
}

void output_label(InterCode code) {
	fputs("LABEL ", fp);
	output_operand(code->u.uniop.op);
	fputs(" :\n", fp);
}

void output_return(InterCode code) {
	fputs("RETURN ", fp);
	output_operand(code->u.uniop.op);
	fputs("\n", fp);
}

void output_goto(InterCode code) {
	fputs("GOTO ", fp);
	output_operand(code->u.uniop.op);
	fputs("\n", fp);
}

void output_call(InterCode code) {
	output_operand(code->u.assign.left);
	fputs(" := CALL ", fp);
	output_operand(code->u.assign.right);
	fputs("\n", fp);
}

void output_dec(InterCode code) {
	fputs("DEC ", fp);
	output_operand(code->u.assign.left);
	fputs(" ", fp);
	fprintf(fp, "%d", code->u.assign.right->u.value);
	fputs("\n", fp);
}

void output_arg(InterCode code) {
	fputs("ARG ", fp);
	//output_operand(code->u.uniop.op);
	if(code->u.uniop.op->flag == 1)
		fprintf(fp, "t%d", code->u.uniop.op->u.temp_no);
	else
		output_operand(code->u.uniop.op);
	fputs("\n", fp);
}

void output_relop(InterCode code) {
	fputs("IF ", fp);
	output_operand(code->u.cond.relop1);
	output_reltype(code->u.cond.reltype);
	output_operand(code->u.cond.relop2);
	fputs(" GOTO ", fp);
	output_operand(code->u.cond.label);
	fputs("\n", fp);
}

void output_read(InterCode code) {
	fputs("READ ", fp);
	output_operand(code->u.uniop.op);
	fputs("\n", fp);
}

void output_write(InterCode code) {
	fputs("WRITE ", fp);
	output_operand(code->u.uniop.op);
	fputs("\n", fp);
}

void output_function(InterCode code) {
	fputs("FUNCTION ", fp);
	output_operand(code->u.uniop.op);
	fputs(" :\n", fp);
}

void output_param(InterCode code) {
	fputs("PARAM ", fp);
	output_operand(code->u.uniop.op);
	fputs("\n", fp);
}

void output_ir(InterCodes ir, char *name) {
	fp = fopen(name, "w+");   
	InterCodes irtable = ir;
	while(irtable != NULL){
		switch(irtable->code->kind){
		case IR_ASSIGN:
			output_assign(irtable->code);
			break;
		case IR_ADD:
			output_add(irtable->code);
			break;
		case IR_SUB:
			output_sub(irtable->code);
			break;
		case IR_MUL:
			output_mul(irtable->code);
			break;
		case IR_DIV:
			output_div(irtable->code);
			break;
		case IR_LABEL:
			output_label(irtable->code);
			break;
		case IR_RETURN:
			output_return(irtable->code);
			break;
		case IR_GOTO:
			output_goto(irtable->code);
			break;
		case IR_CALL: 
			output_call(irtable->code);
			break;
		case IR_DEC:
			output_dec(irtable->code);
			break;
		case IR_ARG:
			output_arg(irtable->code);
			break;
		case IR_RELOP:
			output_relop(irtable->code);
			break;
		case IR_READ:
			output_read(irtable->code);
			break;
		case IR_WRITE:
			output_write(irtable->code);
			break;
		case IR_FUNCTION:
			output_function(irtable->code);
			break;
		case IR_PARAM:
			output_param(irtable->code);
			break;
		}
		irtable = irtable->next;
		//if(irtable == ir)
		//	irtable = NULL;
	}
	fclose(fp);
}

//generate
Operand new_variable(int id) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_VARIABLE;
	op->u.var_no = id;
	op->flag = 0;
	return op;
}

Operand new_temp() {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_TEMP;
	op->u.temp_no = ++temp_count;
	op->flag = 0;
	return op;
}

Operand new_temp_id(int id) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_TEMP;
	op->u.temp_no = id;
	op->flag = 0;
	return op;
}

Operand new_constant(int value) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_CONSTANT;
	op->u.value = value;
	op->flag = 0;
	return op;
}

Operand new_reference(int id) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_REFERENCE;
	op->u.addr_no = id;
	op->flag = 0;
	return op;
}

Operand new_label() {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_LABEL;
	op->u.label_no = ++label_count;
	op->flag = 0;
	return op;
}

Operand new_function(char *func_name) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_FUNCTION;
	op->u.func_name = func_name;
	op->flag = 0;
	return op;
}

Operand new_param(char *param_name) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_PARAM;
	op->u.param_name = param_name;
	op->flag = 0;
	return op;
}

InterCodes gen_assign(IrType kind, Operand left, Operand right) {
	InterCodes icode = (InterCodes)malloc(sizeof(struct InterCodes_));
	icode->prev = icode->next = NULL;
	icode->code = (InterCode)malloc(sizeof(struct InterCode_));
	icode->code->kind = kind;
	icode->code->u.assign.left = left;
	icode->code->u.assign.right = right;
	return icode;
}

InterCodes gen_binop(IrType kind, Operand result, Operand op1, Operand op2) {
	InterCodes icode = (InterCodes)malloc(sizeof(struct InterCodes_));
	icode->prev = icode->next = NULL;
	icode->code = (InterCode)malloc(sizeof(struct InterCode_));
	icode->code->kind = kind;
	icode->code->u.binop.result = result;
	icode->code->u.binop.op1 = op1;
	icode->code->u.binop.op2 = op2;
	return icode;
}

InterCodes gen_uniop(IrType kind, Operand op) {
	InterCodes icode = (InterCodes)malloc(sizeof(struct InterCodes_));
	icode->prev = icode->next = NULL;
	icode->code = (InterCode)malloc(sizeof(struct InterCode_));
	icode->code->kind = kind;
	icode->code->u.uniop.op = op;
	return icode;
}

InterCodes gen_cond(RelType type, Operand label, Operand op1, Operand op2) {
	InterCodes icode = (InterCodes)malloc(sizeof(struct InterCodes_));
	icode->prev = icode->next = NULL;
	icode->code = (InterCode)malloc(sizeof(struct InterCode_));
	icode->code->kind = IR_RELOP;
	icode->code->u.cond.reltype = type;
	icode->code->u.cond.label = label;
	icode->code->u.cond.relop1 = op1;
	icode->code->u.cond.relop2 = op2;
	return icode;
}

//translate
InterCodes translate_Exp(struct Node * node, Operand place) {
	struct Node * child = node->child;
	assert(child != NULL);
	InterCodes code1, code2, code3, code4, code5;
	//INT
	if(child->tokentype == _INT){
		Operand c1 = new_constant(atoi(child->name));
		//if(place != NULL)		
			code1 = gen_assign(IR_ASSIGN, place, c1);
		return code1;
	}
	//ID
	if(child->tokentype == _ID && child->brother == NULL){		
		int id  = getVarID(child);
		Operand v1 = new_variable(id);
		//if(place != NULL)
			code1 = gen_assign(IR_ASSIGN, place, v1);
		return code1;
	}
	//Exp1 ASSIGNOP Exp2
	if(child->tokentype == _Exp && child->brother->tokentype == _ASSIGNOP){
		//ID = Exp2
		if(child->child->tokentype == _ID){
			Operand t1 = new_temp();
			int id = getVarID(child->child);	
			Operand v1 = new_variable(id);
			code1 = translate_Exp(child->brother->brother, t1);
			code2 = gen_assign(IR_ASSIGN, v1, t1);
			code1 = link_ir(code1, code2);
			if(place != NULL){
				code3 = gen_assign(IR_ASSIGN, place, v1);
				code1 = link_ir(code1, code3);
			}
			return code1;
		}
		//Exp[Exp] = Exp2
		//Exp.ID = Exp2
		Operand t1 = new_temp();
		code1 = translate_Exp(child, t1);
		Operand t2 = new_temp();
		code2 = translate_Exp(child->brother->brother, t2);
		code3 = gen_assign(IR_ASSIGN, t1, t2);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code3);
		if(place != NULL){
			code4 = gen_assign(IR_ASSIGN, place, t1);
			code1 = link_ir(code1, code4);
		}
		return code1;
	}
	//Exp PLUS/MINUS/STAR/DIV Exp
	if(child->tokentype == _Exp && 
		(child->brother->tokentype == _PLUS || child->brother->tokentype == _MINUS ||
		child->brother->tokentype == _STAR || child->brother->tokentype == _DIV)){
		Operand t1 = new_temp();
		Operand t2 = new_temp();
		code1 = translate_Exp(child, t1);
		code2 = translate_Exp(child->brother->brother, t2);
		if(child->brother->tokentype == _PLUS)
			code3 = gen_binop(IR_ADD, place, t1, t2);
		else if(child->brother->tokentype == _MINUS)
			code3 = gen_binop(IR_SUB, place, t1, t2);
		else if(child->brother->tokentype == _STAR)
			code3 = gen_binop(IR_MUL, place, t1, t2);
		else if(child->brother->tokentype == _DIV)
			code3 = gen_binop(IR_DIV, place, t1, t2);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code3);
		return code1;
	}
	//MINUS Exp
	if(child->tokentype == _MINUS){
		Operand t1 = new_temp();
		code1 = translate_Exp(child->brother, t1);
		Operand c1 = new_constant(0);
		code2 = gen_binop(IR_SUB, place, c1, t1);
		code1 = link_ir(code1, code2);
		return code1;
	}
	//Exp RELOP Exp
	//NOT Exp
	//Exp AND Exp
	//Exp OR Exp
	if(child->tokentype == _NOT || child->brother->tokentype == _RELOP ||
		child->brother->tokentype == _AND || child->brother->tokentype == _OR){
		Operand label1 = new_label();
		Operand label2 = new_label();
		Operand c1 = new_constant(0);
		Operand c2 = new_constant(1);
		code1 = gen_assign(IR_ASSIGN, place, c1);
		code2 = translate_Cond(node, label1, label2);
		code3 = gen_uniop(IR_LABEL, label1);
		code4 = gen_assign(IR_ASSIGN, place, c2);
		code5 = gen_uniop(IR_LABEL, label2);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code3);
		code1 = link_ir(code1, code4);
		code1 = link_ir(code1, code5);
		return code1;
	}
	//ID LP RP
	if(child->brother->tokentype == _LP && child->brother->brother->tokentype == _RP){
		char *func_name = child->name;
		if(strcmp(func_name, "read") == 0){
			if(place == NULL){
				Operand t1 = new_temp();
				code1 = gen_uniop(IR_READ, t1);
			}
			else
				code1 = gen_uniop(IR_READ, place);
			return code1;
		}
		struct FuncMsg * func_node = getFuncMsg(child);
		assert(func_node != NULL);
		Operand f1 = new_function(func_name);
		if(place != NULL && place->kind != OP_ADDRESS)
			code1 = gen_assign(IR_CALL, place, f1);
		else if(place != NULL && place->kind == OP_ADDRESS){
			Operand t2 = new_temp();
			code1 = gen_assign(IR_CALL, t2, f1);
			code2 = gen_assign(IR_ASSIGN, place, t2);
			code1 = link_ir(code1, code2);
		}
		else{
			Operand t2 = new_temp();
			code1 = gen_assign(IR_CALL, t2, f1);
		}
		return code1;
	}
	//ID LP Args RP
	if(child->brother->tokentype == _LP && child->brother->brother->tokentype == _Args){
		char *func_name = child->name;
		Operand *arg_list = (Operand *)malloc(sizeof(Operand) * 10);
		int arg_num = 0;
		code1 = translate_Args(child->brother->brother, arg_list, &arg_num);
		if(strcmp(func_name, "write") == 0){
			assert(arg_num == 1);
			Operand t1;
			if(arg_list[0]->kind == OP_ADDRESS){
				t1 = new_temp();
				code2 = gen_assign(IR_ASSIGN, t1, arg_list[0]);
				code1 = link_ir(code1, code2);
			}
			else
				t1 = arg_list[0];
			code3 = gen_uniop(IR_WRITE, t1);
			code1 = link_ir(code1, code3);
			return code1;
		}
		int i;
		//for(i=arg_num-1; i>=0; i--){
		for(i=0; i<arg_num; i++){
			code2 = gen_uniop(IR_ARG, arg_list[i]);
			code1 = link_ir(code1, code2);
		}
		Operand f1 = new_function(func_name);
		if(place != NULL && place->kind != OP_ADDRESS)
			code3 = gen_assign(IR_CALL, place, f1);
		else if(place != NULL && place->kind == OP_ADDRESS){
			Operand t2 = new_temp();
			code1 = gen_assign(IR_CALL, t2, f1);
			code2 = gen_assign(IR_ASSIGN, place, t2);
			code1 = link_ir(code1, code2);
		}
		else{
			Operand t2 = new_temp();
			code3 = gen_assign(IR_CALL, t2, f1);
		}
		code1 = link_ir(code1, code3);
		return code1;
	}
	//LP Exp RP
	if(child->tokentype == _LP){
		return translate_Exp(child->brother, place);
	}
	//Exp LB Exp RB
	if(child->brother->tokentype == _LB){
		return translate_array(node, place, NULL, NULL);
	}
	//Exp DOT ID
	if(child->brother->tokentype == _DOT){
		return translate_structure(node, place, NULL);
	}
	return NULL;
}

InterCodes translate_Stmt(struct Node * node) {
	struct Node * child = node->child;
	assert(child != NULL);
	InterCodes code1, code2, code3, code4, code5, code6, code7;
	//Exp SEMI
	if(child->tokentype == _Exp){
		return translate_Exp(child, NULL);
	}
	//CompSt
	if(child->tokentype == _CompSt){
		return translate_Compst(child);
	}
	//RETURN Exp SEMI
	if(child->tokentype == _RETURN){
		Operand t1 = new_temp();
		code1 = translate_Exp(child->brother, t1);
		code2 = gen_uniop(IR_RETURN, t1);
		code1 = link_ir(code1, code2);
		return code1;
	}
	//WHILE LP Exp RP Stmt
	if(child->tokentype == _WHILE){
		Operand label1 = new_label();
		Operand label2 = new_label();
		Operand label3 = new_label();
		code1 = translate_Cond(child->brother->brother, label2, label3);
		code2 = translate_Stmt(child->brother->brother->brother->brother);
		code3 = gen_uniop(IR_LABEL, label1);
		code4 = gen_uniop(IR_LABEL, label2);
		code5 = gen_uniop(IR_LABEL, label3);
		code6 = gen_uniop(IR_GOTO, label1);
		code1 = link_ir(code3, code1);
		code1 = link_ir(code1, code4);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code6);
		code1 = link_ir(code1, code5);
		return code1;
	}
	//IF LP Exp RP Stmt
	if(child->tokentype == _IF && child->brother->brother->brother->brother->brother == NULL){
		Operand label1 = new_label();
		Operand label2 = new_label();
		code1 = translate_Cond(child->brother->brother, label1, label2);
		code2 = translate_Stmt(child->brother->brother->brother->brother);
		code3 = gen_uniop(IR_LABEL, label1);
		code4 = gen_uniop(IR_LABEL, label2);
		code1 = link_ir(code1, code3);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code4);
		return code1;
	}
	//IF LP Exp RP Stmt ELSE Stmt
	if(child->tokentype == _IF  && child->brother->brother->brother->brother->brother != NULL){
		Operand label1 = new_label();
		Operand label2 = new_label();
		Operand label3 = new_label();
		code1 = translate_Cond(child->brother->brother, label1, label2);
		code2 = translate_Stmt(child->brother->brother->brother->brother);
		code3 = translate_Stmt(child->brother->brother->brother->brother->brother->brother);
		code4 = gen_uniop(IR_LABEL, label1);
		code5 = gen_uniop(IR_LABEL, label2);
		code6 = gen_uniop(IR_LABEL, label3);
		code7 = gen_uniop(IR_GOTO, label3);
		code1 = link_ir(code1, code4);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code7);
		code1 = link_ir(code1, code5);
		code1 = link_ir(code1, code3);
		code1 = link_ir(code1, code6);
		return code1;
	}
	return NULL;
}

InterCodes translate_Cond(struct Node * node, Operand label_true, Operand label_false) {
	struct Node * child = node->child;
	assert(child != NULL);
	InterCodes code1, code2, code3, code4;
	if(child->tokentype == _NOT){
		return translate_Cond(child->brother, label_false, label_true);
	}
	if(child->brother != NULL && child->brother->tokentype == _RELOP){
		Operand t1 = new_temp();
		Operand t2 = new_temp();
		code1 = translate_Exp(child, t1);
		code2 = translate_Exp(child->brother->brother, t2);
		RelType reltype = get_relop(child->brother);
		code3 = gen_cond(reltype, label_true, t1, t2);
		code4 = gen_uniop(IR_GOTO, label_false);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code3);
		code1 = link_ir(code1, code4);
		return code1;
	}
	if(child->brother != NULL && child->brother->tokentype == _AND){
		Operand label1 = new_label();
		code1 = translate_Cond(child, label1, label_false);
		code2 = translate_Cond(child->brother->brother, label_true, label_false);
		code3 = gen_uniop(IR_LABEL, label1);
		code1 = link_ir(code1, code3);
		code1 = link_ir(code1, code2);
		return code1;
	}
	if(child->brother != NULL && child->brother->tokentype == _OR){
		Operand label1 = new_label();
		code1 = translate_Cond(child, label_true, label1);
		code2 = translate_Cond(child->brother->brother, label_true, label_false);
		code3 = gen_uniop(IR_LABEL, label1);
		code1 = link_ir(code1, code3);
		code1 = link_ir(code1, code2);
		return code1;
	}
	Operand t1 = new_temp();
	code1 = translate_Exp(node, t1);
	Operand c1 = new_constant(0);
	code2 = gen_cond(NE, label_true, t1, c1);
	code3 = gen_uniop(IR_GOTO, label_false);
	code1 = link_ir(code1, code2);
	code1 = link_ir(code1, code3);
	return code1;
}

InterCodes translate_Compst(struct Node * node) {
	InterCodes code1 = NULL, code2 = NULL;
	struct Node * child = node->child;
	assert(child != NULL);
	//LC DefList StmtList RC
	child = child->brother;
	if(child != NULL && child->tokentype == _DefList){
		code1 = translate_Deflist(child);
		child = child->brother;
	}
	if(child != NULL && child->tokentype != _RC)
		code2 = translate_Stmtlist(child);
	code1 = link_ir(code1, code2);
	return code1;
}

InterCodes translate_Stmtlist(struct Node * node) {
	InterCodes code1, code2;
	//NULL
	if(node == NULL)
		return NULL;
	//Stmt StmtList
	struct Node * child = node->child;
	assert(child != NULL);
	code1 = translate_Stmt(child);
	code2 = translate_Stmtlist(child->brother);
	code1 = link_ir(code1, code2);
	return code1;
}

InterCodes translate_Args(struct Node * node, Operand *arg_list, int *arg_num) {
	InterCodes code1, code2, code3, code4;
	struct Node * child = node->child;
	assert(child != NULL);
	//Exp
	if(child->brother == NULL){
		//Operand t1 = new_temp();
		//code1 = translate_Exp(child, t1);
		if(child->child->tokentype == _ID && child->child->brother == NULL){
			int id = getVarID(child->child);
			enum VarType vtp;
			union Varp vp = getVar(child->child, &vtp);
			if(vtp == V_ARRAY || vtp  == V_STRUCT){
				Operand r1 = new_reference(id);
				Operand t1 = new_temp();
				code1 = gen_assign(IR_ASSIGN, t1, r1);
				t1->flag = 1;
				arg_list[(*arg_num)++] = t1;
				return code1;
			}
		}
		Operand t1 = new_temp();
		code1 = translate_Exp(child, t1);
		if(t1->kind == OP_ADDRESS && t1->flag != 2){		
			t1->flag = 1;
		}
		arg_list[(*arg_num)++] = t1;
		return code1;
	}
	//Exp COMMA Args
	if(child->brother->tokentype == _COMMA){
		if(child->child->tokentype == _ID){
			int  id = getVarID(child->child);
			enum VarType vtp;
			union Varp vp = getVar(child->child, &vtp);
			if(vtp == V_ARRAY || vtp == V_STRUCT){
				Operand r1 = new_reference(id);
				Operand t1 = new_temp();
				code1 = gen_assign(IR_ASSIGN, t1, r1);
				t1->flag = 1;
				//printf("t%d\n", t1->u.temp_no);
				arg_list[(*arg_num)++] = t1;
			}
			else{
				Operand t1 = new_temp();
				code1 = translate_Exp(child, t1);
				arg_list[(*arg_num)++] = t1;
			}
		}
		else{
			Operand t1 = new_temp();
			code1 = translate_Exp(child, t1);
			arg_list[(*arg_num)++] = t1;
		}
		code2 = translate_Args(child->brother->brother, arg_list, arg_num);
		code1 = link_ir(code1, code2);
		return code1;
	}
}

InterCodes translate_Extdef(struct Node * node) {
	InterCodes code1, code2;
	struct Node * child = node->child;
	assert(child != NULL);
	//Specifier ExtDecList SEMI
	if(child->brother->tokentype == _ExtDecList){
		return translate_Extdeclist(child->brother);
	}
	//Specifier FunDec CompSt
	if(child->brother->tokentype == _FunDec){
		code1 = translate_Fundec(child->brother);
		code2 = translate_Compst(child->brother->brother);
		code1 = link_ir(code1, code2);
		return code1;
	}
	//Specifier SEMI
	return NULL;
}

InterCodes translate_Extdeclist(struct Node * node) {
	InterCodes code1, code2;
	struct Node * child = node->child;
	assert(child != NULL);
	//VarDec
	if(child->brother == NULL){
//		printf("translate_Extdeclist V:%s\n", child->name);
		return translate_Vardec(child, NULL);
	}
	//VarDec COMMA ExtDecList
	if(child->brother != NULL && child->brother->tokentype == _COMMA){
//		printf("translate_Extdeclist V C E:%s\n", child->name);
		code1 = translate_Vardec(child, NULL);
		code2 = translate_Extdeclist(child->brother->brother);
		code1 = link_ir(code1, code2);
		return code1;
	}
	return NULL;
}

InterCodes translate_Fundec(struct Node * node) {
	InterCodes code1, code2;
	struct Node * child = node->child;
	assert(child != NULL);
	char *func_name = child->name;
	struct FuncMsg* func_node = getFuncMsg(child);
	assert(func_node != NULL);
	Operand f1 = new_function(func_name);
	code1 = gen_uniop(IR_FUNCTION, f1);
	//ID LP VarList RP
	if(child->brother->brother->brother != NULL){
		struct ParList* param = func_node->arglist;
		while(param != NULL){
			//Operand p1 = new_param(param->name);
			//if(param->type->kind == basic)
			if(param->partype == V_ARRAY){
				printf("Can not translate the code: \n");
				printf("Contain multidimensional array and function parameters of array type!\n");
				exit(-1);
			}
			
			Operand p1 = new_variable((getVarIDbyName(param->name)));
			code2 = gen_uniop(IR_PARAM, p1);
			code1 = link_ir(code1, code2);
			param = param->next;
		}
		return code1;
	}
	//ID LP RP
	return code1;
}

InterCodes translate_Vardec(struct Node * node, Operand *place) {
	InterCodes code1, code2, code3, code4;
//	printf("node is %s\n",node->name);
	struct Node * child = node->child;
	assert(child != NULL);
	//ID
	if(child->tokentype == _ID){
		int id = getVarID(child);
		enum VarType vtp;
		union Varp vp = getVar(child, &vtp);
		Operand v1 = new_variable(id);
		if(place != NULL)
			*place = v1;
		if(vtp == V_INT || vtp == V_FLOAT){
			return NULL;//No need to output anything
		}
		else if(vtp == V_ARRAY || vtp == V_STRUCT){
			//malloc size
			int size = get_structure_size(vp, vtp);
			Operand c1 = new_constant(size);
			code1 = gen_assign(IR_DEC, v1, c1);
			return code1;
		}else{
			printf("vtp is VERROR?%d\n", vtp == V_ERROR);
			exit(-1);
		}
	}
	//VarDec LB INT RB
//	printf("translate_Vardec : node is %s, child tokentype is _ID? %d\n", child->name, child->tokentype == _ID);
	return translate_Vardec(child, NULL);
}

InterCodes translate_Deflist(struct Node * node) {
	//NULL
	if(node == NULL)
		return NULL;
	//Def DefList
	InterCodes code1, code2;
	struct Node * child = node->child;
	assert(child != NULL);
	code1 = translate_Def(child);
	code2 = translate_Deflist(child->brother);
	code1 = link_ir(code1, code2);
	return code1;
}

InterCodes translate_Def(struct Node * node) {
	struct Node * child = node->child;
//	printf("translate_Def:%s\n", node->name);
	assert(child != NULL);
	//Specifier DecList SEMI
//	printf("translate_Def: to declist %s\n", child->brother->name);
	return translate_Declist(child->brother);
}

InterCodes translate_Declist(struct Node * node) {
	InterCodes code1, code2;
	struct Node * child = node->child;
	assert(child != NULL);
	//Dec
	if(child->brother == NULL){
		return translate_Dec(child);
	}
	//Dec COMMA DecList
	if(child->brother != NULL && child->brother->tokentype == _COMMA){
		code1 = translate_Dec(child);
		code2 = translate_Declist(child->brother->brother);
		code1 = link_ir(code1, code2);
		return code1;
	}
}

InterCodes translate_Dec(struct Node * node) {
	InterCodes code1, code2, code3;
	struct Node * child = node->child;
	assert(child != NULL);
	//VarDec
	if(child->brother == NULL){
//		printf("translate_Dec V:%s\n", child->name);
		return translate_Vardec(child, NULL);
	}
	//VarDec ASSIGNOP Exp
	if(child->brother != NULL && child->brother->tokentype == _ASSIGNOP){
		Operand t1 = NULL;
//		printf("translate_Dec V A E:%s\n", child->name);
		code1 = translate_Vardec(child, &t1);
		Operand t2 = new_temp();
		code2 = translate_Exp(child->brother->brother, t2);
		code3 = gen_assign(IR_ASSIGN, t1, t2);
		code1 = link_ir(code1, code2);
		code1 = link_ir(code1, code3);
		return code1;
	}
	return NULL;
}

InterCodes translate_structure(struct Node * node, Operand place, symnode_t **list) {
	struct Node * child = node->child;
	assert(child != NULL);
	InterCodes code1, code2;
	int offset = 0;
	//Exp DOT ID
	//ID.ID
	if(child->child->tokentype == _ID){
		int id = getVarID(child->child);
		enum VarType vtp;
		union Varp vp = getVar(child->child, &vtp);
		int flag =  getFlag(child->child->name);
		char *member_name = child->brother->brother->name;
		symnode_t * member = vp.sp->memlist;
		while(member != NULL){
			if(strcmp(member->smb.msgp.vmsgp->name, member_name) == 0)
				break;
			offset += get_structure_size(member->smb.msgp.vmsgp->tp, member->smb.msgp.vmsgp->type);
			member = member->layernext;
		}
		assert(member != NULL);
		if(list != NULL)
			*list = member;
		Operand r1;	
		if(flag == 1)
			r1 = new_variable(id);
		else
			r1 = new_reference(id);
		//printf("%s\n", id_node->name);
		Operand c1 = new_constant(offset); 
		Operand t1 = new_temp_id(place->u.temp_no);
		code1 = gen_binop(IR_ADD, t1, r1, c1);
		place->kind = OP_ADDRESS;
		return code1;
	}
	//Exp[Exp].ID
	else if(child->child->brother->tokentype == _LB){
		Operand t1 = new_temp();
		union Varp id_type;
		enum VarType mvtp;
		code1 = translate_array(child, t1, &id_type, &mvtp);
		//assert(t1->kind == OP_ADDRESS);	
		assert(mvtp == V_STRUCT);
		char *member_name = child->brother->brother->name;
		symnode_t* member = id_type.sp->memlist;
		while(member != NULL){
			if(strcmp(member->smb.msgp.vmsgp->name, member_name) == 0)
				break;
			offset += get_structure_size(member->smb.msgp.vmsgp->tp, member->smb.msgp.vmsgp->type);
			member = member->layernext;
		}
		assert(member != NULL);
		if(list != NULL)
			*list = member;
		Operand c1 = new_constant(offset); 
		Operand t2 = new_temp_id(place->u.temp_no);
		Operand t3 = new_temp_id(t1->u.temp_no);
		code2 = gen_binop(IR_ADD, t2, t3, c1);
		code1 = link_ir(code1, code2);
		place->kind = OP_ADDRESS;
		return code1;
	}
	//Exp.ID.ID
	else if(child->child->brother->tokentype == _DOT){
		Operand t1 = new_temp();
		symnode_t *id_node;
		code1 = translate_structure(child, t1, &id_node);
		assert(t1->kind == OP_ADDRESS);
		assert(id_node != NULL);
		assert(id_node->smb.msgp.vmsgp->type == V_STRUCT);
		char *member_name = child->brother->brother->name;
		symnode_t * member = id_node->smb.msgp.vmsgp->tp.sp->memlist;
		while(member != NULL){
			if(strcmp(member->smb.msgp.vmsgp->name, member_name) == 0)
				break;
			offset += get_structure_size(member->smb.msgp.vmsgp->tp, member->smb.msgp.vmsgp->type);
			member = member->layernext;
		}
		assert(member != NULL);
		if(list != NULL)
			*list = member;
		Operand c1 = new_constant(offset); 
		Operand t2 = new_temp_id(place->u.temp_no);
		Operand t3 = new_temp_id(t1->u.temp_no);
		code2 = gen_binop(IR_ADD, t2, t3, c1);
		code1 = link_ir(code1, code2);
		place->kind = OP_ADDRESS;
		return code1;
	}
	return NULL;
}

InterCodes translate_array(struct Node * node, Operand place, union Varp *list, enum VarType *listvtp){
	struct Node * child = node->child;
	assert(child != NULL);
	InterCodes code1 = NULL;
	InterCodes code2, code3, code4;
	int size = 0;
	Operand t1;
	//ID[Exp]
	if(child->child->tokentype == _ID){
		int id = getVarID(child->child);	
		int flag = getFlag(child->child->name);
		if(flag == 1)
			t1 = new_variable(id);
		else
			t1 = new_reference(id);
		enum VarType vtp;
		union Varp vp = getVar(child->child, &vtp);
		if(vp.ap->basetype == V_STRUCT){
			size = get_structure_size(vp.ap->base, vp.ap->basetype);
			//place->kind = OP_ADDRESS;
		}
		else if(vp.ap->basetype == V_INT || vp.ap->basetype == V_FLOAT){
			size = 4;
			place->flag = 2;//array element is basic
		}
		if(list != NULL){
			*list = vp.ap->base;
			*listvtp = vp.ap->basetype;
		}
	}
	//Exp.ID[Exp]
	else if(child->child->brother != NULL && child->child->brother->tokentype == _DOT){
		t1 = new_temp();
		symnode_t *id_node;
		code1 = translate_structure(child, t1, &id_node);
		t1->kind = OP_TEMP;
		size = get_structure_size(id_node->smb.msgp.vmsgp->tp.ap->base, id_node->smb.msgp.vmsgp->tp.ap->basetype);
		if(size == 4)
			place->flag = 2;//array element is basic
		if(list != NULL){
			*list = id_node->smb.msgp.vmsgp->tp.ap->base;
			*listvtp = id_node->smb.msgp.vmsgp->tp.ap->basetype;
		}
	}
	//Exp[Exp][Exp]
	else if(child->child->brother != NULL && child->child->brother->tokentype == _LB){
		printf("Can not translate the code: Contain multidimensional array and function parameters of array type!\n");
		exit(-1);
	}
	//handle Exp2
	Operand t2 = new_temp();
	code2 = translate_Exp(child->brother->brother, t2);
	code1 = link_ir(code1, code2);
	//Calculate offset
	Operand c1 = new_constant(size);
	Operand t3 = new_temp();
	code3 = gen_binop(IR_MUL, t3, t2, c1);
	code1 = link_ir(code1, code3);
	//Add the array address
	Operand t4 = new_temp_id(place->u.temp_no);
	code4 = gen_binop(IR_ADD, t4, t1, t3);
	code1 = link_ir(code1, code4);
	place->kind = OP_ADDRESS;
	return code1;
}

//optimize
int optimize_ir(InterCodes *ir) {
	InterCodes curcode = *ir;
	InterCodes nextcode, prevcode;
	Operand op1, op2, op3, op4;
	int *labellist = (int *)malloc(sizeof(int) * 20);
	int value;
	char *name;
	int count = 0;

	if(ir == NULL || *ir == NULL)
		return 0;
	while(curcode->next != NULL)
		curcode = curcode->next;
	while(curcode != NULL){
		switch(curcode->code->kind){
		case IR_ASSIGN: 
			break;
		case IR_ADD: 
		case IR_SUB: 
		case IR_MUL:
		case IR_DIV:
			op1 = curcode->code->u.binop.op1;
			op2 = curcode->code->u.binop.op2;
			if(op1->kind == OP_TEMP && op2->kind != OP_TEMP){
				prevcode = curcode->prev;
				if(prevcode->code->kind == IR_ASSIGN){
					op3 = prevcode->code->u.assign.left;
					if(op3->kind == OP_TEMP && op3->u.temp_no == op1->u.temp_no){
						curcode->code->u.binop.op1 = prevcode->code->u.assign.right;
						remove_ir(ir, prevcode);
					}
				}
			}
			else if(op2->kind == OP_TEMP && op1->kind != OP_TEMP){
				prevcode = curcode->prev;
				if(prevcode->code->kind == IR_ASSIGN){
					op3 = prevcode->code->u.assign.left;
					if(op3->kind == OP_TEMP && op3->u.temp_no == op2->u.temp_no){
						curcode->code->u.binop.op2 = prevcode->code->u.assign.right;
						remove_ir(ir, prevcode);
					}
				}
			}
			else if(op1->kind == OP_TEMP && op2->kind == OP_TEMP){
				prevcode = *ir;
				while(prevcode != curcode){
					if(prevcode->code->kind == IR_ASSIGN){
						op3 = prevcode->code->u.assign.left;
						if(op3->kind == OP_TEMP && op3->u.temp_no == op1->u.temp_no){
							curcode->code->u.binop.op1 = prevcode->code->u.assign.right;
							remove_ir(ir, prevcode);
						}
						else if(op3->kind == OP_TEMP && op3->u.temp_no == op2->u.temp_no){
							curcode->code->u.binop.op2 = prevcode->code->u.assign.right;
							remove_ir(ir, prevcode);
						}
					}
					prevcode = prevcode->next;
				}
			}
			break;
		case IR_LABEL: 
			value = curcode->code->u.uniop.op->u.label_no;
			prevcode = curcode->prev;
			while(prevcode->code->kind == IR_LABEL){
				assert(prevcode->code->u.uniop.op->kind == OP_LABEL);
				labellist[count++] = prevcode->code->u.uniop.op->u.label_no;
				remove_ir(ir, prevcode);
				prevcode = prevcode->prev;
			}
			//printf("Count: %d\n", count);
			if(count > 0){
				prevcode = *ir;
				while(prevcode != curcode){
					int i;
					if(prevcode->code->kind == IR_GOTO){
						for(i=0; i<count; i++){
							//printf("list %d: %d\n", i, labellist[i]);
							//printf("label: %d\n", prevcode->code->u.uniop.op->u.label_no);
							if(prevcode->code->u.uniop.op->u.label_no == labellist[i]){
								//printf("Match: %d\n", labellist[i]);
								prevcode->code->u.uniop.op->u.label_no = value;
								//remove_ir(ir, prevcode);
								break;
							}
						}
					}
					else if(prevcode->code->kind == IR_RELOP){
						for(i=0; i<count; i++){
							if(prevcode->code->u.cond.label->u.label_no == labellist[i]){
								prevcode->code->u.cond.label->u.label_no = value;
								//remove_ir(ir, prevcode);
								break;
							}
						}
					}
					prevcode = prevcode->next;
				}
			}
			break;
		case IR_READ:
			nextcode = curcode->next;
			if(nextcode->code->kind == IR_ASSIGN){
				op1 = curcode->code->u.uniop.op;
				op2 = nextcode->code->u.assign.right;
				if(op1->kind == OP_TEMP && op2->kind == OP_TEMP && op1->u.temp_no == op2->u.temp_no){
					curcode->code->u.uniop.op = nextcode->code->u.assign.left;
					remove_ir(ir, nextcode);
				}
			}
			break;
		case IR_RETURN:
		case IR_ARG:
		case IR_WRITE:
			op1 = curcode->code->u.uniop.op;
			if(op1->kind == OP_TEMP){
				prevcode = *ir;
				while(prevcode != curcode){
					if(prevcode->code->kind == IR_ASSIGN){
						op2 = prevcode->code->u.assign.left;
						if(op2->kind == OP_TEMP && op2->u.temp_no == op1->u.temp_no){
							curcode->code->u.uniop.op = prevcode->code->u.assign.right;
							remove_ir(ir, prevcode);
						}
					}
					else if(prevcode->code->kind == IR_ADD || prevcode->code->kind == IR_SUB ||
						prevcode->code->kind == IR_MUL || prevcode->code->kind == IR_DIV){
						op2 = prevcode->code->u.binop.op1;
						op3 = prevcode->code->u.binop.op2;
						if(op2->kind == OP_CONSTANT && op3->kind == OP_CONSTANT){
							if(prevcode->code->kind == IR_ADD)
								value = op2->u.value + op3->u.value;
							else if(prevcode->code->kind == IR_SUB)
								value = op2->u.value - op3->u.value;
							else if(prevcode->code->kind == IR_MUL)
								value = op2->u.value * op3->u.value;
							else
								value = op2->u.value / op3->u.value;
							op4 = prevcode->code->u.binop.result;
							if(op4->kind == OP_TEMP && op4->u.temp_no == op1->u.temp_no){
								curcode->code->u.uniop.op->kind = OP_CONSTANT;
								curcode->code->u.uniop.op->u.value = value;
								remove_ir(ir, prevcode);
							}
						}
					}
					prevcode = prevcode->next;
				}
			}
			break;
		case IR_CALL:
			/*nextcode = curcode->next;
			if(nextcode->code->kind == IR_ASSIGN){
				op1 = curcode->code->u.assign.left;
				op2 = nextcode->code->u.assign.right;
				if(op1->kind == OP_TEMP && op2->kind == OP_TEMP && op1->u.temp_no == op2->u.temp_no){
					curcode->code->u.assign.left = nextcode->code->u.assign.left;
					remove_ir(ir, nextcode);
				}
			}*/
			break;
		case IR_RELOP:
			op1 = curcode->code->u.cond.relop1;
			op2 = curcode->code->u.cond.relop2;
			if(op1->kind == OP_TEMP && op2->kind != OP_TEMP){
				prevcode = curcode->prev;
				if(prevcode->code->kind == IR_ASSIGN){
					op3 = prevcode->code->u.assign.left;
					if(op3->kind == OP_TEMP && op3->u.temp_no == op1->u.temp_no){
						curcode->code->u.cond.relop1 = prevcode->code->u.assign.right;
						remove_ir(ir, prevcode);
					}
				}
			}
			else if(op2->kind == OP_TEMP && op1->kind != OP_TEMP){
				prevcode = curcode->prev;
				if(prevcode->code->kind == IR_ASSIGN){
					op3 = prevcode->code->u.assign.left;
					if(op3->kind == OP_TEMP && op3->u.temp_no == op2->u.temp_no){
						curcode->code->u.cond.relop2 = prevcode->code->u.assign.right;
						remove_ir(ir, prevcode);
					}
				}
			}
			else if(op1->kind == OP_TEMP && op2->kind == OP_TEMP){
				prevcode = *ir;
				while(prevcode != curcode){
					if(prevcode->code->kind == IR_ASSIGN){
						op3 = prevcode->code->u.assign.left;
						if(op3->kind == OP_TEMP && op3->u.temp_no == op1->u.temp_no){
							curcode->code->u.cond.relop1 = prevcode->code->u.assign.right;
							remove_ir(ir, prevcode);
						}
						else if(op3->kind == OP_TEMP && op3->u.temp_no == op2->u.temp_no){
							curcode->code->u.cond.relop2 = prevcode->code->u.assign.right;
							remove_ir(ir, prevcode);
						}
					}
					prevcode = prevcode->next;
				}
			}
			break;
		case IR_GOTO:
			break; 
		case IR_DEC:
			break;
		case IR_FUNCTION:
			name = curcode->code->u.uniop.op->u.func_name;
			prevcode = curcode->prev;
			if(prevcode != NULL && prevcode->code->kind == IR_LABEL){
				assert(prevcode->code->u.uniop.op->kind == OP_LABEL);
				value = prevcode->code->u.uniop.op->u.label_no;
				remove_ir(ir, prevcode);
				//prevcode = prevcode->prev;
				prevcode = *ir;
				while(prevcode != curcode){
					int i;
					if(prevcode->code->kind == IR_GOTO){
						if(prevcode->code->u.uniop.op->u.label_no == value){
							prevcode->code->u.uniop.op->kind = OP_FUNCTION;
							prevcode->code->u.uniop.op->u.func_name = name;
							break;
						}
					}
					else if(prevcode->code->kind == IR_RELOP){
						if(prevcode->code->u.cond.label->u.label_no == value){
							prevcode->code->u.cond.label->kind = OP_FUNCTION;
							prevcode->code->u.cond.label->u.func_name = name;
							break;
						}
					}
					prevcode = prevcode->next;
				}
			}
			break;
		case IR_PARAM:
			break;
		}
		curcode = curcode->prev;
	}
	return 1;
}

//other functions
InterCodes link_ir(InterCodes l1, InterCodes l2) {
	InterCodes tmp;
	if(l1 == NULL) return l2;
	if(l2 == NULL) return l1;
	tmp = l1;
	while(tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = l2;
	l2->prev = tmp;
	return l1;
}

int remove_ir(InterCodes *table, InterCodes icode) {
	if(table == NULL || *table == NULL)
		return -1;
	InterCodes irhead = *table;
	while(irhead != NULL){
		if(irhead == icode){
			if(irhead->prev == NULL){
				*table = irhead->next;
				(*table)->prev = NULL;
			}
			else{
				irhead->next->prev = irhead->prev;
				irhead->prev->next = irhead->next;
			}
			free(irhead);
			return 1;
		}
		irhead = irhead->next;
	}
	printf("Cannot find the icode to be removed\n");
	exit(-1);
}

int get_structure_size(union Varp type, enum VarType vtp) {
	int size = 0;
	if(vtp == V_ERROR)
		return 0;
	if(vtp == V_INT || vtp == V_FLOAT)
		return 4;
	if(vtp == V_STRUCT){
		symnode_t *mem = type.sp->memlist;
		while(mem != NULL){
			size += get_structure_size(mem->smb.msgp.vmsgp->tp, mem->smb.msgp.vmsgp->type);
			mem = mem->layernext;
		}
		return size;
	}
	if(vtp == V_ARRAY){
		size = type.ap->size * get_structure_size(type.ap->base, type.ap->basetype);
		return size;
	}
	return 0;
}

RelType get_relop(struct Node * node) {
	char *value = node->name;
	if(strcmp(value, "==") == 0) return EQ;
	else if(strcmp(value, "!=") == 0) return NE;
	else if(strcmp(value, ">") == 0) return MT;
	else if(strcmp(value, "<") == 0) return LT;
	else if(strcmp(value, ">=") == 0) return ME;
	else if(strcmp(value, "<=") == 0) return LE;
	else{
		printf("Cannot match relop type!\n");
		exit(-1);
	}
}

