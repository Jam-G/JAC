#include"intercode.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int Treg_table[10];
int Sreg_table[9];
int Areg_table[4];
int arg_num;//计算需要的$a总数
int arg_index;//标记当前传入的参数的是第几个
int globl;//标记当前是否在main函数中：main（1）

void iniitize_table()//初始化所有寄存器
{
	int i;
	for(i=0;i<10;i++)
		Treg_table[i]=0;
	for(i=0;i<9;i++)
		Sreg_table[i]=0;
	for(i=0;i<4;i++)
		Areg_table[i]=0;
	arg_num = 0;
	arg_index = 0;
	globl = 0;
}
extern int temp_count;
void push_Areg(int var_no)
{
	Areg_table[arg_num] = var_no;
}

int choose_Treg(int temp_no)
{
	int i=0;
	for(i;i<10;i++)
	{	
		if(Treg_table[i] == 0)//有空的寄存器
		{
			Treg_table[i] = temp_no;
			return i;
		}
		if(Treg_table[i] == temp_no)//临时变量已经存在于某个寄存器中
			return i;
	}
	//如果没有空的寄存器，而且临时变量不存在于10个$t寄存器中，则取一个寄存器替换(FIFO)
	int index = (temp_count-1)%10;
	Treg_table[index] = temp_no;	
	return index;
}
extern int temp_count;
int choose_Sreg(int var_no)
{
	int i;
	if(globl == 0)
	{
		for(i=0;i<arg_num;i++)
			if(Areg_table[i] == var_no)
				return 10+i;
	}
	//printf("var_no:%d \n",var_no);
	
	for(i=0;i<8;i++)
	{	
		if(Sreg_table[i] == 0)//有空的寄存器
		{
			Sreg_table[i] = var_no;
			return i;
		}
		if(Sreg_table[i] == var_no)//临时变量已经存在于某个寄存器中
			return i;
	}
	//如果没有空的寄存器，而且临时变量不存在于8个$s0~$s7寄存器中，则取一个寄存器替换(FIFO).
	int index = (temp_count-1)%8;
	Sreg_table[index] = var_no;
	return index;
}

int check_R(InterCodes head)
{
	InterCodes temp = head;
	while(temp!=NULL)
	{
		if(temp->code->kind == IR_READ)
			return 1;
		temp = temp->next;
	}
	return 0;
}

int check_W(InterCodes head)
{
	InterCodes temp = head;
	while(temp!=NULL)
	{
		if(temp->code->kind == IR_WRITE)
			return 1;
		temp = temp->next;
	}
	return 0;
}

void print_reg(Operand op,FILE *fp)
{
	int index;
	switch(op->kind)
	{
		case OP_VARIABLE:
			index = choose_Sreg(op->u.var_no);
			if(index<10)
				fprintf(fp,"$s%d",index);
			else
				fprintf(fp,"$a%d",index-10);
			break;
		case OP_TEMP:
			fprintf(fp,"$t%d",choose_Treg(op->u.temp_no));break;
		case OP_CONSTANT:
			fprintf(fp,"%d",op->u.value);
			break;
	//	case FLOAT_T:
//			fprintf(fp,"%lf",op->u.float_value);break;
		case OP_ADDRESS:
			fprintf(fp,"$v%d",op->u.var_no);break;
		default:
			fprintf(fp,"Operand->kind unKnown!");break;
	}
}

//翻译单条InterCodes到MIPS代码
void print_IR(InterCode code,FILE *fp)
{
	switch(code->kind)
	{
		case IR_ASSIGN:
			fprintf(fp,"	");
			if(code->u.assign.right->kind == OP_CONSTANT)
				fprintf(fp,"li ");
			else if(code->u.assign.right->kind == OP_ADDRESS)
				fprintf(fp,"la ");
			else	
				fprintf(fp,"move ");	
			print_reg(code->u.assign.left,fp);
			fprintf(fp,", ");		
			print_reg(code->u.assign.right,fp);
			break;
		case IR_ADD:
			if(code->u.assign.right->kind == OP_CONSTANT)
				fprintf(fp,"	addi ");
			else
				fprintf(fp,"	add ");
			print_reg(code->u.binop.result,fp);
			fprintf(fp,", ");
			print_reg(code->u.binop.op1,fp);
			fprintf(fp,", ");
			print_reg(code->u.binop.op2,fp);
			break;
		case IR_SUB:
			fprintf(fp,"	");
			if(code->u.binop.op2->kind == OP_CONSTANT)//地址？
				fprintf(fp,"addi ");
			else
				fprintf(fp,"sub ");
			print_reg(code->u.binop.result,fp);
			fprintf(fp,", ");
			if(code->u.binop.op1->kind == OP_CONSTANT && code->u.binop.op1->u.value == 0)
				fprintf(fp,"$0 ");
			else
				print_reg(code->u.binop.op1,fp);
			fprintf(fp,", ");
			if(code->u.binop.op2->kind == OP_CONSTANT){//地址？
				fprintf(fp,"-");
				print_reg(code->u.binop.op2,fp);
			}
			else
				print_reg(code->u.binop.op2,fp);
			break;
		case IR_MUL:
			fprintf(fp,"	mul ");
			print_reg(code->u.binop.result,fp);
			fprintf(fp,", ");
			print_reg(code->u.binop.op1,fp);
			fprintf(fp,", ");
			print_reg(code->u.binop.op2,fp);
			break;
		case IR_DIV:
			fprintf(fp,"	div ");
			print_reg(code->u.binop.op1,fp);
			fprintf(fp,", ");
			print_reg(code->u.binop.op2,fp);
			fprintf(fp,"\n	");
			fprintf(fp,"mflo ");
			print_reg(code->u.binop.result,fp);
			break;
		/*case REM:
			print_operand(code->u.binop.result,fp);
			fprintf(fp," := ");
			print_operand(code->u.binop.op1,fp);
			fprintf(fp," mod ");
			print_operand(code->u.binop.op2,fp);
			break;*/
		/*case POINT:
			fprintf(fp,"	lw ");
			print_reg(code->u.uniop.result,fp);
			fprintf(fp,", 0(");
			print_reg(code->u.uniop.op,fp);	
			fprintf(fp,")");
			break;
		case ADDR:
			fprintf(fp,"	sw ");
			print_reg(code->u.uniop.op,fp);
			fprintf(fp,", 0(");
			print_reg(code->u.uniop.result,fp);	
			fprintf(fp,")");
			break;
			*/
		case IR_FUNCTION:
			if(strcmp(code->u.uniop.op->u.func_name,"main") == 0)
				globl = 1;
			else
				globl = 0;
			fprintf(fp,"%s: ",code->u.uniop.op->u.func_name);
			break;			
		case IR_PARAM://***
			push_Areg(code->u.uniop.op->u.var_no);
			arg_num ++;
			arg_index ++;
			//fprintf(fp,"PARAM ");
			//print_operand(code->u.param.parameter,fp);
			break;
		case IR_ARG://***
			if(globl == 0)
			{
				if(arg_index == arg_num)
					fprintf(fp,"	addi $sp, $sp, -%d \n",4*arg_index+4);
				fprintf(fp,"	sw $a%d, %d($sp) \n",arg_index-1,4*(arg_num-arg_index));			
			}
			else
				fprintf(fp,"	addi $sp, $sp, -4 \n");
		//	arg_index--;
			fprintf(fp,"	move $a%d, ",arg_num - arg_index);
			arg_index--;
			print_reg(code->u.uniop.op,fp);
			break;
		case IR_RETURN:
			if(code->u.uniop.op->kind == OP_CONSTANT){
				fprintf(fp, "	li $v0, ");
			}else
				fprintf(fp,"	move $v0, ");
			if(code->u.uniop.op->kind == OP_CONSTANT && code->u.uniop.op->u.value == 0)
				fprintf(fp,"$0 ");
			else
				print_reg(code->u.uniop.op,fp);
			fprintf(fp,"\n");
			fprintf(fp,"	jr $ra");
			break;
		case IR_CALL:
			//将返回地址压入栈中
			if(arg_num == 0)
			{
				fprintf(fp,"	addi $sp, $sp, -4 \n");
				fprintf(fp,"	sw $ra, 0($sp) \n");		
			}
			else if(globl)//main中调用有参数函数的时候
				fprintf(fp,"	sw $ra, 0($sp) \n");
			else
				fprintf(fp,"	sw $ra, %d($sp) \n",4*arg_num);
			//跳转到调用的函数处
			fprintf(fp,"	jal %s\n",code->u.assign.right->u.func_name);
			//将返回地址和参数出栈，恢复现场
			if(arg_num == 0 || globl == 1)
			{
				fprintf(fp,"	lw $ra, 0($sp) \n");
				fprintf(fp,"	addi $sp, $sp, 4 \n");
			}
			else
			{
				fprintf(fp,"	lw $ra, %d($sp) \n",4*arg_num);
				assert(arg_index == 0);
				for(arg_index;arg_index<arg_num;arg_index++)
					fprintf(fp,"	lw $a%d, %d($sp) \n",arg_index,4*(arg_num-arg_index-1));			
				fprintf(fp,"	addi $sp, $sp, %d \n",4*arg_index+4);
			}
			//获取函数返回值
			fprintf(fp,"	move ");
			print_reg(code->u.assign.left,fp);
			fprintf(fp,", $v0 ");			
			break;
		case IR_LABEL:
			fprintf(fp,"label%d :",code->u.uniop.op->u.label_no);
			break;
		case IR_GOTO:
		//	if()
			fprintf(fp,"	j label%d ",code->u.uniop.op->u.label_no);
			break;
		case IR_RELOP:
			if(code->u.cond.reltype == EQ)
				fprintf(fp,"	beq ");
			else if(code->u.cond.reltype == NE)
				fprintf(fp,"	bne ");
			else if(code->u.cond.reltype == MT)
				fprintf(fp,"	bgt ");
			else if(code->u.cond.reltype == LT)
				fprintf(fp,"	blt ");
			else if(code->u.cond.reltype == ME)
				fprintf(fp,"	bge ");
			else if(code->u.cond.reltype == LE)
				fprintf(fp,"	ble ");
			else 
				fprintf(fp,"unKnown relop ");
			print_reg(code->u.cond.relop1,fp);
			fprintf(fp,", ");
			print_reg(code->u.cond.relop2,fp);
			fprintf(fp,", label%d ",code->u.cond.label->u.label_no);
			break;
		case IR_DEC://****
			fprintf(fp,"DEC ");
			print_reg(code->u.uniop.op,fp);
			fprintf(fp," %d",code->u.uniop.op->u.value);
			break;
		case IR_READ:
			fprintf(fp,"	addi $sp, $sp,-4 \n");
			fprintf(fp,"	sw $ra 0($sp) \n");
			fprintf(fp,"	jal read \n");
			fprintf(fp,"	lw $ra 0($sp) \n");
			fprintf(fp,"	addi $sp, $sp,4 \n");
			fprintf(fp,"	move ");
			print_reg(code->u.uniop.op,fp);
			fprintf(fp,", $v0 ");
			break;	
		case IR_WRITE:
			if(code->u.uniop.op->kind == OP_CONSTANT){//地址？
				fprintf(fp,"	li $a0, ");
				print_reg(code->u.uniop.op,fp);
				fprintf(fp,"\n");
			}
			else{
				fprintf(fp,"	move $a0, ");//
				print_reg(code->u.uniop.op, fp);
				fprintf(fp,"\n");
			}			
			fprintf(fp,"	addi $sp, $sp,-4 \n");
			fprintf(fp,"	sw $ra 0($sp) \n");
			fprintf(fp,"	jal write \n");
			fprintf(fp,"	lw $ra 0($sp) \n");
			fprintf(fp,"	addi $sp, $sp,4 ");
			break;
		default:
			fprintf(fp,"code->kind unKnown:%d \n",code->kind);
			break;
	}
	fprintf(fp,"\n");
}
extern InterCodes irtable;
void print_IRs(char *filename)
{
	iniitize_table();
//	int nameLen = strlen(filename);
//	filename[nameLen-2] = 's';
//	filename[nameLen-1] = '\0';

	FILE *fp = fopen(filename,"w");	
	printf("filename:%s \n",filename);

	InterCodes temp = irtable;
	int read = check_R(irtable);
	int write = check_W(irtable);
	if(temp == NULL)
	{
		printf("the interCodes is not exist! \n");
		return;	
	}
	else
	{
		fprintf(fp,".data \n");
		if(read)
			fprintf(fp,"_prompt: .asciiz \"Enter an integer:\" \n");
		if(write)
			fprintf(fp,"_ret: .asciiz \"\\n\" \n");
		while(temp->code->kind != IR_FUNCTION)
		{
			print_IR(temp->code,fp);
			temp = temp->next;
		}	
		
		fprintf(fp,".globl main \n");
		fprintf(fp,".text \n");		
		if(read)
		{	
			fprintf(fp,"read: \n");
			//打印一串字符，syscall 4
			fprintf(fp,"	li $v0, 4 \n");
			fprintf(fp,"	la $a0, _prompt \n");
			fprintf(fp,"	syscall \n");
			//读取一个整数，syscall 5
			fprintf(fp,"	li $v0, 5 \n");
			fprintf(fp,"	syscall \n");
			fprintf(fp,"	jr $ra \n");
		}
		if(write)
		{	
			fprintf(fp,"write: \n");
			//打印一个整数，syscall 1，参数在调用处填入$a0
			fprintf(fp,"	li $v0, 1 \n");
			fprintf(fp,"	syscall \n");
			//打印一串字符（换行符），syscall 4
			fprintf(fp,"	li $v0, 4 \n");
			fprintf(fp,"	la $a0, _ret \n");
			fprintf(fp,"	syscall \n");
			//填充返回值			
			fprintf(fp,"	move $v0, $0 \n");
			fprintf(fp,"	jr $ra \n");
		}
	}	
	while(temp != NULL)
	{
		print_IR(temp->code,fp);
		temp = temp->next;
	}
}
