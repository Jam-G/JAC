.data 
_prompt: .asciiz "Enter an integer:" 
_ret: .asciiz "\n" 
.globl main 
.text 
read: 
	li $v0, 4 
	la $a0, _prompt 
	syscall 
	li $v0, 5 
	syscall 
	jr $ra 
write: 
	li $v0, 1 
	syscall 
	li $v0, 4 
	la $a0, _ret 
	syscall 
	move $v0, $0 
	jr $ra 
fact: 

	beq $a0, 1, label1 
	j label2 
label1 :
	move $v0, $a0
	jr $ra
	j label3 
label2 :
	addi $t0, $a0, -1
	addi $sp, $sp, -8 
	sw $a0, 0($sp) 
	move $a0, $t0
	sw $ra, 4($sp) 
	jal fact
	lw $ra, 4($sp) 
	lw $a0, 0($sp) 
	addi $sp, $sp, 8 
	move $t1, $v0 
	mul $t2, $a0, $t1
	move $v0, $t2
	jr $ra
label3 :
main: 
	addi $sp, $sp,-4 
	sw $ra 0($sp) 
	jal read 
	lw $ra 0($sp) 
	addi $sp, $sp,4 
	move $s0, $v0 
	bgt $s0, 1, label4 
	j label5 
label4 :
	addi $sp, $sp, -4 
	move $a0, $s0
	sw $ra, 0($sp) 
	jal fact
	lw $ra, 0($sp) 
	addi $sp, $sp, 4 
	move $t3, $v0 
	move $s1, $t3
	j label6 
label5 :
	li $t4, 1
	move $s1, $t4
label6 :
	move $a0, $s1
	addi $sp, $sp,-4 
	sw $ra 0($sp) 
	jal write 
	lw $ra 0($sp) 
	addi $sp, $sp,4 
	move $v0, $0 
	jr $ra
