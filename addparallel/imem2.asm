add r2, r1, r0, 1       # r2 = 1.
sla r10, r2, r1, 12     # r10 = 4096. The offset of the second array
sla r2, r2, r1, 10      # r2 = 1024. The offset to finish copy in.
add r3, r0, r1, 2048    # r3 = 2048. r3 shall hold the block offset in memory we calculate the sum for.
a: lw r4, r3, r1, 0     # r4 shall be used to copy the first word in the block
lw r5, r3, r1, 1        # r5 shall be used to copy the second word in the block
lw r6, r3, r1, 2        # r6 shall be used to copy the third word in the block
lw r7, r3, r1, 3        # r7 shall be used to copy the fourth word in the block
add r8, r3, r10, 0      # r8 = index in array 2
lw r9, r8, r1, 0        # r9 is a register which shall be used to load words from second array
add r4, r4, r9, 0
lw r9, r8, r1, 1
add r5, r5, r9, 0
lw r9, r8, r1, 2
add r6, r6, r9, 0
lw r9, r8, r1, 3
add r7, r7, r9, 0
add r8, r8, r10, 0      # r8 = index in array 3
sw r4, r8, r1, 0
sw r5, r8, r1, 1
sw r6, r8, r1, 2
add r3, r3, r1, 4
bne r1, r3, r2, a       # unlike the one core calculation, in each core one of every four blocks is calculated
sw r7, r8, r1, 3
halt r0, r0, r0, 0