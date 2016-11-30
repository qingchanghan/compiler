.data
temp: .word 0x7fff0000
string0: .asciiz "fun: gbi:"
string1: .asciiz "fun: c:"
string2: .asciiz "fun2: gbinta:"
string3: .asciiz "fun2: c: no such variable"
string4: .asciiz "fun3: return a"
string5: .asciiz "n1 >= MAX"
string6: .asciiz "n2 != MIN"
string7: .asciiz "fun1: gbi:"
string8: .asciiz "n3 <= M_MAX"
string9: .asciiz "fun3: C1:"
string10: .asciiz "n4 <= M_MIN"
string11: .asciiz "c1 and C1 are not equal."
string12: .asciiz "2 is true."
string13: .asciiz "0 is true."
string14: .asciiz "0 is false."
string15: .asciiz "c5 - c1 is true."
string16: .asciiz "c5 - c1 is false."
.text
lw $t8, temp
move $t9, $sp
subi $fp, $sp, 12

# CONST INT || CONST CHAR
li $t0, 23
sw $t0, -12($sp)
subi $fp, $sp, 16

# CONST INT || CONST CHAR
li $t0, 1
sw $t0, -16($sp)
subi $fp, $sp, 20

# CONST INT || CONST CHAR
li $t0, 43
sw $t0, -20($sp)
subi $fp, $sp, 24

# INT || CHAR
subi $fp, $sp, 424

# INT || CHAR
subi $fp, $sp, 428

# INT || CHAR
subi $fp, $sp, 448

# INT || CHAR
subi $fp, $sp, 452

j main

# FUNC
fun:
sw $ra, 0($fp)
sw $sp, -4($fp)
move $sp, $fp
subi $fp, $fp, 12

# PAR INT || PAR CHAR
subi $fp, $sp, 16

# PAR INT || PAR CHAR
subi $fp, $sp, 20

# INT || CHAR
subi $fp, $sp, 24

# = 
lw $t0, -12($sp)
sw $t0, 0($t8)

# = 
lw $t0, 0($t8)
sw $t0, -424($t9)

# = 
lw $t0, -424($t9)
sw $t0, -4($t8)

# = 
lw $t0, -12($sp)
sw $t0, -8($t8)

# = 
lw $t0, -8($t8)
sw $t0, -20($sp)

# = 
lw $t0, -20($sp)
sw $t0, -12($t8)

# label
label0:

# = 
lw $t0, -20($sp)
sw $t0, -16($t8)

# = 
li $t0, 1
sw $t0, -20($t8)

# + - * /
lw $t0, -20($t8)
lw $t1, -16($t8)
add $t2, $t0, $t1
sw $t2, -24($t8)

# = 
lw $t0, -24($t8)
sw $t0, -20($sp)

# = 
lw $t0, -20($sp)
sw $t0, -28($t8)

# = 
lw $t0, -424($t9)
sw $t0, -32($t8)

# = 
lw $t0, -20($sp)
sw $t0, -36($t8)

# + - * /
lw $t0, -36($t8)
lw $t1, -32($t8)
add $t2, $t0, $t1
sw $t2, -40($t8)

# = 
lw $t0, -40($t8)
sw $t0, -424($t9)

# = 
lw $t0, -424($t9)
sw $t0, -44($t8)

# = 
lw $t0, -20($sp)
sw $t0, -48($t8)

# = 
lw $t0, -12($t9)
sw $t0, -52($t8)

# compare
lw $t0, -48($t8)
lw $t1, -48($t8)
blt $t0, $t1, label0

# WRITE STRING
la $a0, string0
li $v0, 4
syscall

# = 
lw $t0, -424($t9)
sw $t0, -56($t8)

# WRITE
lw $a0, -56($t8)
li $v0, 1
syscall

# WRITE STRING
la $a0, string1
li $v0, 4
syscall

# = 
lw $t0, -16($sp)
sw $t0, -60($t8)

# WRITE
lw $a0, -60($t8)
li $v0, 11
syscall

# FUNC
fun1:
sw $ra, 0($fp)
sw $sp, -4($fp)
move $sp, $fp
subi $fp, $fp, 12

# PAR INT || PAR CHAR
subi $fp, $sp, 16

# INT || CHAR
subi $fp, $sp, 20

# = 
lw $t0, -12($sp)
sw $t0, 0($t8)

# = 
lw $t0, -12($t9)
sw $t0, -4($t8)

# compare
lw $t0, 0($t8)
lw $t1, 0($t8)
bge $t0, $t1, label1

# = 
lw $t0, -12($sp)
sw $t0, -8($t8)

# = 
li $t0, 1
sw $t0, -12($t8)

# + - * /
lw $t0, -12($t8)
lw $t1, -8($t8)
add $t2, $t0, $t1
sw $t2, -16($t8)

# real par
lw $t0, -16($t8)
sw $t0, -12($fp)

# call function
jal fun1

# =call
sw $v0, -20($t8)

# = 
lw $t0, -12($sp)
sw $t0, -24($t8)

# + - * /
lw $t0, -24($t8)
lw $t1, -20($t8)
add $t2, $t0, $t1
sw $t2, -28($t8)


# RETURN
lw $t0, -28($t8)
sw $t0, -8($sp)
move $fp, $sp
lw $v0, -8($sp)
lw $ra, 0($sp)
lw $sp, -4($sp)
jr $ra

# label
label1:

# = 
lw $t0, -12($sp)
sw $t0, -32($t8)


# RETURN
lw $t0, -32($t8)
sw $t0, -8($sp)
move $fp, $sp
lw $v0, -8($sp)
lw $ra, 0($sp)
lw $sp, -4($sp)
jr $ra

# FUNC
fun2:
sw $ra, 0($fp)
sw $sp, -4($fp)
move $sp, $fp
subi $fp, $fp, 12

# CONST INT || CONST CHAR
li $t0, 5
sw $t0, -12($sp)
subi $fp, $sp, 16

# INT || CHAR
subi $fp, $sp, 20

# = 
lw $t0, -12($sp)
sw $t0, 0($t8)

# = 
lw $t0, -12($sp)
sw $t0, -4($t8)

# + - * /
lw $t0, -4($t8)
lw $t1, 0($t8)
mul $t2, $t0, $t1
sw $t2, -8($t8)

# = 
lw $t0, -12($sp)
sw $t0, -12($t8)

# + - * /
lw $t0, -12($t8)
lw $t1, -8($t8)
div $t2, $t0, $t1
sw $t2, -16($t8)

# = 
lw $t0, -16($t8)
sw $t0, -424($t9)

# = 
lw $t0, -424($t9)
sw $t0, -20($t8)

# = 
lw $t0, -424($t9)
sw $t0, -24($t8)

# = 
lw $t0, -424($t9)
sw $t0, -28($t8)

# + - * /
lw $t0, -28($t8)
lw $t1, -24($t8)
sub $t2, $t0, $t1
sw $t2, -32($t8)

# = 
lw $t0, -32($t8)
sw $t0, -16($sp)

# = 
lw $t0, -16($sp)
sw $t0, -16($sp)

# label
label2:

# = 
lw $t0, -16($sp)
sw $t0, -40($t8)

# = 
lw $t0, -12($sp)
sw $t0, -44($t8)

# compare
lw $t0, -40($t8)
lw $t1, -40($t8)
bge $t0, $t1, label3

# = 
lw $t0, -16($sp)
sw $t0, -48($t8)

# = 
lw $t0, -16($sp)
sw $t0, -52($t8)

# []=
lw $t0, -52($t8)
lw $t1, -48($t8)
sll $t1,$t1,2
subi $t2, $t9, 420
sub $t2, $t2, $t1
sw $t0, 0($t2)

# =[]
lw $t1, -48($t8)
sll $t1,$t1,2
subi $t2, $t9, 420
sub $t2, $t2, $t1
lw $t0, 0($t2)
sw $t0, -56($t8)

# WRITE STRING
la $a0, string2
li $v0, 4
syscall

# = 
lw $t0, -16($sp)
sw $t0, -60($t8)

# =[]
lw $t1, -60($t8)
sll $t1,$t1,2
subi $t2, $t9, 420
sub $t2, $t2, $t1
lw $t0, 0($t2)
sw $t0, -64($t8)

# WRITE
lw $a0, -64($t8)
li $v0, 1
syscall

# + - * /
lw $t0, -16($sp)
add $t2, $t0, $t1
sw $t2, -16($sp)

# j
j label2

# label
label3:

# WRITE STRING
la $a0, string3
li $v0, 4
syscall

# FUNC
fun3:
sw $ra, 0($fp)
sw $sp, -4($fp)
move $sp, $fp
subi $fp, $fp, 12

# WRITE STRING
la $a0, string4
li $v0, 4
syscall

# = 
li $t0, 97
sw $t0, 0($t8)


# RETURN
lw $t0, 0($t8)
sw $t0, -8($sp)
move $fp, $sp
lw $v0, -8($sp)
lw $ra, 0($sp)
lw $sp, -4($sp)
jr $ra

# FUNC
main:

# CONST INT || CONST CHAR
li $t0, 20
sw $t0, -12($sp)
subi $fp, $sp, 16

# CONST INT || CONST CHAR
li $t0, 10
sw $t0, -16($sp)
subi $fp, $sp, 20

# INT || CHAR
subi $fp, $sp, 24

# INT || CHAR
subi $fp, $sp, 28

# INT || CHAR
subi $fp, $sp, 32

# INT || CHAR
subi $fp, $sp, 36

# INT || CHAR
subi $fp, $sp, 40

# INT || CHAR
subi $fp, $sp, 44

# INT || CHAR
subi $fp, $sp, 48

# INT || CHAR
subi $fp, $sp, 52

# INT || CHAR
subi $fp, $sp, 56

# INT || CHAR
subi $fp, $sp, 60

# INT || CHAR
subi $fp, $sp, 460

# INT || CHAR
subi $fp, $sp, 480

# = 
li $t0, 20
sw $t0, 0($t8)

# = 
lw $t0, 0($t8)
sw $t0, -20($sp)

# = 
lw $t0, -20($sp)
sw $t0, -4($t8)

# = 
li $t0, 98
sw $t0, -8($t8)

# = 
lw $t0, -8($t8)
sw $t0, -36($sp)

# = 
lw $t0, -36($sp)
sw $t0, -12($t8)

# = 
li $t0, 2
sw $t0, -16($t8)

# = 
lw $t0, -16($t8)
sw $t0, -24($sp)

# = 
lw $t0, -24($sp)
sw $t0, -20($t8)

# = 
li $t0, 99
sw $t0, -24($t8)

# = 
lw $t0, -24($t8)
sw $t0, -40($sp)

# = 
lw $t0, -40($sp)
sw $t0, -28($t8)

# = 
li $t0, 21
sw $t0, -32($t8)

# = 
lw $t0, -32($t8)
sw $t0, -28($sp)

# = 
lw $t0, -28($sp)
sw $t0, -36($t8)

# = 
li $t0, 100
sw $t0, -40($t8)

# = 
lw $t0, -40($t8)
sw $t0, -44($sp)

# = 
lw $t0, -44($sp)
sw $t0, -44($t8)

# = 
li $t0, 10
sw $t0, -48($t8)

# = 
lw $t0, -48($t8)
sw $t0, -32($sp)

# = 
lw $t0, -32($sp)
sw $t0, -52($t8)

# = 
li $t0, 101
sw $t0, -56($t8)

# = 
lw $t0, -56($t8)
sw $t0, -48($sp)

# = 
lw $t0, -48($sp)
sw $t0, -60($t8)

# = 
li $t0, 102
sw $t0, -64($t8)

# = 
lw $t0, -64($t8)
sw $t0, -52($sp)

# = 
lw $t0, -52($sp)
sw $t0, -68($t8)

# = 
lw $t0, -20($sp)
sw $t0, -72($t8)

# = 
lw $t0, -12($t9)
sw $t0, -76($t8)

# compare
lw $t0, -72($t8)
lw $t1, -72($t8)
bge $t0, $t1, label4

# = 
lw $t0, -20($sp)
sw $t0, -80($t8)

# = 
lw $t0, -36($sp)
sw $t0, -84($t8)

# real par
lw $t0, -80($t8)
sw $t0, -12($fp)

# real par
lw $t0, -84($t8)
sw $t0, -16($fp)

# call function
jal fun

# j
j label5

# label
label4:

# WRITE STRING
la $a0, string5
li $v0, 4
syscall

# label
label5:

# = 
lw $t0, -24($sp)
sw $t0, -88($t8)

# = 
lw $t0, -16($t9)
sw $t0, -92($t8)

# compare
lw $t0, -88($t8)
lw $t1, -88($t8)
beq $t0, $t1, label6

# WRITE STRING
la $a0, string6
li $v0, 4
syscall

# j
j label7

# label
label6:

# = 
lw $t0, -24($sp)
sw $t0, -96($t8)

# real par
lw $t0, -96($t8)
sw $t0, -12($fp)

# call function
jal fun1

# =call
sw $v0, -100($t8)

# = 
lw $t0, -100($t8)
sw $t0, -424($t9)

# = 
lw $t0, -424($t9)
sw $t0, -104($t8)

# WRITE STRING
la $a0, string7
li $v0, 4
syscall

# = 
lw $t0, -424($t9)
sw $t0, -108($t8)

# WRITE
lw $a0, -108($t8)
li $v0, 1
syscall

# label
label7:

# = 
lw $t0, -28($sp)
sw $t0, -112($t8)

# = 
lw $t0, -12($sp)
sw $t0, -116($t8)

# compare
lw $t0, -112($t8)
lw $t1, -112($t8)
ble $t0, $t1, label8

# WRITE STRING
la $a0, string8
li $v0, 4
syscall

# j
j label9

# label
label8:

# = 
lw $t0, -28($sp)
sw $t0, -120($t8)

# = 
lw $t0, -44($sp)
sw $t0, -124($t8)

# real par
lw $t0, -120($t8)
sw $t0, -12($fp)

# real par
lw $t0, -124($t8)
sw $t0, -16($fp)

# call function
jal fun

# = 
lw $t0, -28($sp)
sw $t0, -128($t8)

# real par
lw $t0, -128($t8)
sw $t0, -12($fp)

# call function
jal fun1

# label
label9:

# = 
lw $t0, -32($sp)
sw $t0, -132($t8)

# = 
lw $t0, -16($sp)
sw $t0, -136($t8)

# compare
lw $t0, -132($t8)
lw $t1, -132($t8)
blt $t0, $t1, label10

# call function
jal fun3

# =call
sw $v0, -140($t8)

# = 
lw $t0, -140($t8)
sw $t0, -56($sp)

# = 
lw $t0, -56($sp)
sw $t0, -144($t8)

# WRITE STRING
la $a0, string9
li $v0, 4
syscall

# = 
lw $t0, -56($sp)
sw $t0, -148($t8)

# WRITE
lw $a0, -148($t8)
li $v0, 11
syscall

# label
label10:

# = 
lw $t0, -32($sp)
sw $t0, -152($t8)

# = 
lw $t0, -16($sp)
sw $t0, -156($t8)

# compare
lw $t0, -152($t8)
lw $t1, -152($t8)
bgt $t0, $t1, label11

# WRITE STRING
la $a0, string10
li $v0, 4
syscall

# label
label11:

# = 
lw $t0, -36($sp)
sw $t0, -160($t8)

# = 
lw $t0, -56($sp)
sw $t0, -164($t8)

# compare
lw $t0, -160($t8)
lw $t1, -160($t8)
beq $t0, $t1, label12

# WRITE STRING
la $a0, string11
li $v0, 4
syscall

# label
label12:

# = 
li $t0, 2
sw $t0, -168($t8)

# compare
lw $t0, -168($t8)
bne $t0, $t1, label13

# WRITE STRING
la $a0, string12
li $v0, 4
syscall

# label
label13:

# = 
li $t0, 0
sw $t0, -172($t8)

# compare
lw $t0, -172($t8)
bne $t0, $t1, label14

# WRITE STRING
la $a0, string13
li $v0, 4
syscall

# j
j label15

# label
label14:

# WRITE STRING
la $a0, string14
li $v0, 4
syscall

# label
label15:

# = 
lw $t0, -52($sp)
sw $t0, -176($t8)

# = 
lw $t0, -36($sp)
sw $t0, -180($t8)

# + - * /
lw $t0, -180($t8)
lw $t1, -176($t8)
sub $t2, $t0, $t1
sw $t2, -184($t8)

# compare
lw $t0, -184($t8)
bne $t0, $t1, label16

# WRITE STRING
la $a0, string15
li $v0, 4
syscall

# j
j label17

# label
label16:

# WRITE STRING
la $a0, string16
li $v0, 4
syscall

# label
label17:

