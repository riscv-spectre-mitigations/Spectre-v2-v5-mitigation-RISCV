.section .text
.global gadget
.global want
.extern end

gadget:

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

want:

nop
j 	end
