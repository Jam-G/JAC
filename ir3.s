.data 
_ret: .asciiz "\n" 
.globl main 
.text 
write: 
	li $v0, 1 
	syscall 
	li $v0, 4 
	la $a0, _ret 
	syscall 
	move $v0, $0 
	jr $ra 
gcd: 


	beq $a1, 0, label1 
	j label2 
label1 :
	move $v0, $a0
	jr $ra
	j label3 
label2 :
	div $a0, $a1
	mflo $t0
	mul $t1, $a1, $t0
	sub $t2, $a0, $t1
	addi $sp, $sp, -12 
	sw $a1, 0($sp) 
	move $a0, $a1
	sw $a0, 4($sp) 
	move $a1, $t2
	sw $ra, 8($sp) 
	jal gcd
	lw $ra, 8($sp) 
	lw $a0, 4($sp) 
	lw $a1, 0($sp) 
	addi $sp, $sp, 12 
	move $t3, $v0 
	move $v0, $t3
	jr $ra
label3 :
main: 
	li $t4, 2013
	move $s0, $t4
	li $t5, 66
	move $s1, $t5
	addi $sp, $sp, -4 
	move $a0, $s0
	addi $sp, $sp, -4 
	move $a1, $s1
	sw $ra, 0($sp) 
	jal gcd
	lw $ra, 0($sp) 
	addi $sp, $sp, 4 
	move $t6, $v0 
	move $a0, $t6
	addi $sp, $sp,-4 
	sw $ra 0($sp) 
	jal write 
	lw $ra 0($sp) 
	addi $sp, $sp,4 
	li $v0, 1
	jr $ra
