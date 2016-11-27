.text
li $t0, 1
sw $t0, 0($sp)
subi $fp, $sp, 12
subi $fp, $fp, 4
subi $fp, $fp, 4
j main

func:
sw $ra, 0($fp)
sw $sp, -4($fp)
move $sp, $fp
subi $fp, $fp, 20
lw $t0, -12($sp)
lw $t1, -16($sp)
add $t2, $t0, $t1
sw $t2, -8($sp)

move $fp, $sp
lw $v0, -8($sp)
lw $ra, 0($sp)
lw $sp, -4($sp)
jr $ra

main:
li $t0, 2
sw $t0, 8($fp)
li $t0, 3
sw $t0, 4($fp)
lw $t0, 8($fp)
sw $t0, -12($fp)
lw $t0, 4($fp)
sw $t0, -16($fp)
jal func
li $t3, 1