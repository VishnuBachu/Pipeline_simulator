/*addi x1,x0,0
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
bne x4 x9 inner*/
# Bubble Sort for 5 elements

main:
    li x1, 0        # x1 = base address of array
    li x2, 5        # x2 = n (array size)

    addi x3, x0, 0  # i = 0

outer_loop:
    addi x4, x0, 0      # j = 0
    addi x5, x2, -1
    sub  x5, x5, x3     # x5 = n-i-1

inner_loop:

    mul x6, x4, x7      # offset = j*4
    add x8, x1, x6      # address = base + offset

    lw x9, 0(x8)        # A[j]
    lw x10, 4(x8)       # A[j+1]

    blt x9, x10, no_swap

    sw x10, 0(x8)       # swap
    sw x9, 4(x8)

no_swap:
    addi x4, x4, 1      # j++

    blt x4, x5, inner_loop

    addi x3, x3, 1      # i++
    blt x3, x2, outer_loop
