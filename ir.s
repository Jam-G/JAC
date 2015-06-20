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
main: 
	li $t0, 0
	move $s0, $t0
	li $t1, 1
	move $s0, $t1
	li $t2, 0
	move $s1, $t2
	addi $sp, $sp,-4 
	sw $ra 0($sp) 
	jal read 
	lw $ra 0($sp) 
	addi $sp, $sp,4 
	move $s2, $v0 
label1 :
	blt $s1, $s2, label2 
	j label3 
label2 :
	add $t3, $s3, $s0
	move $s3, $t3
	move $a0, $s0
	addi $sp, $sp,-4 
	sw $ra 0($sp) 
	jal write 
	lw $ra 0($sp) 
	addi $sp, $sp,4 
	move $t4, $s0
	move $s4, $t4
	move $t5, $s3
	move $s0, $t5
	add $t6, $s1, 1
	move $s1, $t6
	j label1 
label3 :
	move $v0, $0 
	jr $ra
