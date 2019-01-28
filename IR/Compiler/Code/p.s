.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text

read:
subu $sp, $sp, 8
sw $ra, 4($sp)
sw $fp, 0($sp)
addi $fp, $sp, 8
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
subu $sp, $fp, 8
lw $ra, 4($sp)
lw $fp, 0($sp)
jr $ra

write:
subu $sp, $sp, 8
sw $ra, 4($sp)
sw $fp, 0($sp)
addi $fp, $sp, 8
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
subu $sp, $fp, 8
lw $ra, 4($sp)
lw $fp, 0($sp)
move $v0, $0
jr $ra

main:
subu $sp, $sp, 8
sw $ra, 4($sp)
sw $fp, 0($sp)
addi $fp, $sp, 8
subu $s7, $fp, 20
li $s5, 0
move $s6, $s5
li $s4, 4
mul $s3, $s6, $s4
add $s1, $s2, $s3
li $t9, 1
move $s0, $t9
move $t8, $s0
sw $t8, -44($fp)
li $t6, 0
move $t7, $t6
li $t5, 4
mul $t4, $t7, $t5
add $t3, $s2, $t4
move $t2, $t1
subu $v1, $fp, 12
sw $s7, 0($v1)
move $s7, $t2
