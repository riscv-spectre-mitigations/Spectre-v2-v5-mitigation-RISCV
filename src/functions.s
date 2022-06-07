.section .text
.global gadget
.global want

gadget:

addi	sp,sp,-16
sd	ra,8(sp)
sd  	s0,0(sp)
addi	s0,sp,16

la a4, 	array1
lw a5, 	passInIdx
add	a5,a5,a4
lbu	a5,0(a5) 
sext.w	a5,a5
slliw	a5,a5,0x6
sext.w	a5,a5
la a4, 	array2
add	a5,a5,a4
lbu	a5,0(a5)


ld	ra,8(sp)
ld  	s0,0(sp)
addi	sp,sp,16
jr	ra

want:
addi	sp,sp,-16
sd	ra,8(sp)
sd  	s0,0(sp)
addi	s0,sp,16

nop

ld	ra,8(sp)
ld  	s0,0(sp)
addi	sp,sp,16
jr	ra

 
