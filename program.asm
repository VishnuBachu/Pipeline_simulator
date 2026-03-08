addi x1,x0,0
addi x2,x0,5
addi x3,x0,0

outer:
addi x4,x0,0

inner:
add x5,x1,x4

lw x6,0(x5)
lw x7,4(x5)

sub x8,x6,x7
bne x8 x0 swap

noswap:
addi x4,x4,4
addi x9,x2,-1
bne x4 x9 inner

addi x3,x3,1
bne x3 x2 outer

swap:
sw x7,0(x5)
sw x6,4(x5)
addi x4,x4,4
addi x9,x2,-1
bne x4 x9 inner
