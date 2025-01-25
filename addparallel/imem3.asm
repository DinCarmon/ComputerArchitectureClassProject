add $r2, $imm, $zero, 1       # $r2 = 1.
sll $imm0, $r2, $imm, 12     # $imm0 = 4096. The offset of the second array
sll $r2, $r2, $imm, 10      # $r2 = 1024. The offset to finish copy in.
add $r3, $zero, $imm, 3072    # $r3 = 3072. $r3 shall hold the block offset in memory we calculate the sum for.
a:
lw $r4, $r3, $imm, 0     # $r4 shall be used to copy the first word in the block
lw $r5, $r3, $imm, 1        # $r5 shall be used to copy the second word in the block
lw $r6, $r3, $imm, 2        # $r6 shall be used to copy the third word in the block
lw $r7, $r3, $imm, 3        # $r7 shall be used to copy the fourth word in the block
add $r8, $r3, $imm0, 0      # $r8 = index in array 2
lw $r9, $r8, $imm, 0        # $r9 is a $register which shall be used to load words from second array
add $r4, $r4, $r9, 0
lw $r9, $r8, $imm, 1
add $r5, $r5, $r9, 0
lw $r9, $r8, $imm, 2
add $r6, $r6, $r9, 0
lw $r9, $r8, $imm, 3
add $r7, $r7, $r9, 0
add $r8, $r8, $imm0, 0      # $r8 = index in array 3
sw $r4, $r8, $imm, 0
sw $r5, $r8, $imm, 1
sw $r6, $r8, $imm, 2
add $r3, $r3, $imm, 4
bne $imm, $r3, $r2, a       # unlike the one core calculation, in each core one of every four blocks is calculated
sw $r7, $r8, $imm, 3
halt $zero, $zero, $zero, 0